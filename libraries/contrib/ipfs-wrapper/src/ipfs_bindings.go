// Useful links:
// https://github.com/ipfs/go-ipfs/issues/3060
// https://github.com/ipfs/examples/tree/master/examples

package main

import (
	"fmt"
	"context"
	"os"
//	"bytes"
	"sort"
	"unsafe"
	"time"
	"io"
	"io/ioutil"
	"bufio"
	core "github.com/ipfs/go-ipfs/core"
	repo "github.com/ipfs/go-ipfs/repo"
	fsrepo "github.com/ipfs/go-ipfs/repo/fsrepo"
	config "github.com/ipfs/go-ipfs/repo/config"
	path "github.com/ipfs/go-ipfs/path"
	"github.com/ipfs/go-ipfs/core/coreunix"
	corerepo "github.com/ipfs/go-ipfs/core/corerepo"
	"encoding/json"
	bitswap "github.com/ipfs/go-ipfs/exchange/bitswap"
	uio "github.com/ipfs/go-ipfs/unixfs/io"
	unixfspb "github.com/ipfs/go-ipfs/unixfs/pb"
	merkledag "github.com/ipfs/go-ipfs/merkledag"
	unixfs "github.com/ipfs/go-ipfs/unixfs"

	peer "gx/ipfs/QmXYjuNuxVzXKJCfWasQk1RqkhVLDM9jtUKhqc2WPQmFSB/go-libp2p-peer"
	node "gx/ipfs/QmPN7cwmpcc4DWXb4KTB9dNAJgjuPY69h3npsMfhRrQL9c/go-ipld-format"
)

// #cgo CFLAGS: -DIN_GO=1 -ggdb
//#include <stdlib.h>
//#include <stddef.h>
//#include <stdint.h>
//
//// Don't export these functions into C or we'll get "unused function" warnings
//// (Or errors saying functions are defined more than once if the're not static).
//
//#if IN_GO
//static void execute_void_cb(void* func, void* arg)
//{
//    ((void(*)(void*)) func)(arg);
//}
//static void execute_data_cb(void* func, void* data, size_t size, void* arg)
//{
//    ((void(*)(char*, size_t, void*)) func)(data, size, arg);
//}
//#endif // if IN_GO
import "C"

const (
	nBitsForKeypair = 2048
	repoRoot = "./repo"
	debug = false
)

func main() {
}

func doesnt_exist_or_is_empty(path string) bool {
	f, err := os.Open(path)
	if err != nil {
		if os.IsNotExist(err) {
			return true
		}
		return false
	}
	defer f.Close()

	_, err = f.Readdirnames(1)
	if err == io.EOF {
		return true
	}
	return false
}

func openOrCreateRepo(repoRoot string) (repo.Repo, error) {
	if doesnt_exist_or_is_empty(repoRoot) {
		conf, err := config.Init(os.Stdout, nBitsForKeypair)

		if err != nil {
			return nil, err
		}

		if err := fsrepo.Init(repoRoot, conf); err != nil {
			return nil, err
		}
	}

	return fsrepo.Open(repoRoot)
}

func printSwarmAddrs(node *core.IpfsNode) {
	if !node.OnlineMode() {
		fmt.Println("Swarm not listening, running in offline mode.")
		return
	}
	var addrs []string
	for _, addr := range node.PeerHost.Addrs() {
		addrs = append(addrs, addr.String())
	}
	sort.Sort(sort.StringSlice(addrs))

	for _, addr := range addrs {
		fmt.Printf("Swarm listening on %s\n", addr)
	}
}

type Cache struct {
	node *core.IpfsNode
	ctx context.Context
	cancel context.CancelFunc
}

var g Cache

//export go_ipfs_cache_start
func go_ipfs_cache_start(c_repoPath *C.char) bool {
	repoRoot := C.GoString(c_repoPath)

	g.ctx, g.cancel = context.WithCancel(context.Background())

	r, err := openOrCreateRepo(repoRoot);

	if err != nil {
		fmt.Println("err", err);
		return false
	}

	g.node, err = core.NewNode(g.ctx, &core.BuildCfg{
		Online: true,
		Permament: true,
		Repo:   r,
	})
    if err != nil {
		fmt.Println("err", err);
		return false
	}
	g.node.SetLocal(false)

	printSwarmAddrs(g.node)
	return true
}

//export go_ipfs_cache_stop
func go_ipfs_cache_stop() {
	g.cancel()
}

//export go_ipfs_cache_resolve
func go_ipfs_cache_resolve(c_ipns_id *C.char, fn unsafe.Pointer, fn_arg unsafe.Pointer) {
	ipns_id := C.GoString(c_ipns_id)

	go func() {
		if debug {
			fmt.Println("go_ipfs_cache_resolve start");
			defer fmt.Println("go_ipfs_cache_resolve end");
		}

		ctx := g.ctx
		n := g.node
		p := path.Path("/ipns/" + ipns_id)

		node, err := core.Resolve(ctx, n.Namesys, n.Resolver, p)

		if err != nil {
			C.execute_data_cb(fn, nil, C.size_t(0), fn_arg)
			return
		}

		data := []byte(node.Cid().String())
		cdata := C.CBytes(data)
		defer C.free(cdata)

		C.execute_data_cb(fn, cdata, C.size_t(len(data)), fn_arg)
	}()
}

// IMPORTANT: The returned value needs to be explicitly `free`d.
//export go_ipfs_cache_ipns_id
////
func go_ipfs_cache_ipns_id() *C.char {
	pid, err := peer.IDFromPrivateKey(g.node.PrivateKey)

	if err != nil {
		return nil
	}

	cstr := C.CString(pid.Pretty())
	return cstr
}

func publish(ctx context.Context, duration time.Duration, n *core.IpfsNode, cid string) error {
	path, err := path.ParseCidToPath(cid)

	if err != nil {
		fmt.Println("go_ipfs_cache_publish failed to parse cid \"", cid, "\"");
		return err
	}

	k := n.PrivateKey

	eol := time.Now().Add(duration)
	err  = n.Namesys.PublishWithEOL(ctx, k, path, eol)

	if err != nil {
		fmt.Println("go_ipfs_cache_publish failed");
		return err
	}

	return nil
}

//export go_ipfs_cache_publish
func go_ipfs_cache_publish(cid *C.char, seconds C.int64_t, fn unsafe.Pointer, fn_arg unsafe.Pointer) {
	id := C.GoString(cid)

	go func() {
		if debug {
			fmt.Println("go_ipfs_cache_publish start");
			defer fmt.Println("go_ipfs_cache_publish end");
		}

        // https://stackoverflow.com/questions/17573190/how-to-multiply-duration-by-integer
		publish(g.ctx, time.Duration(seconds) * time.Second, g.node, id);
		C.execute_void_cb(fn, fn_arg)
	}()
}

//export go_ipfs_cache_id
func go_ipfs_cache_id() (*C.char) {

    if !g.node.OnlineMode() {
        return nil;
    }

    result := C.CString( g.node.Identity.Pretty() )
    return result;
}

//export go_ipfs_cache_file_add
func go_ipfs_cache_file_add(c_filename *C.char) (bool, *C.char) {
    filename := C.GoString(c_filename)

    if debug {
        fmt.Println("go_ipfs_cache_file_add start");
        defer fmt.Println("go_ipfs_cache_file_add end");
    }

    file,err := os.Open(filename)
    if err != nil {
        fmt.Println("Error: open failed! ", err)
        return false, nil;
    }
    defer file.Close()

    cid, err := coreunix.Add(g.node, bufio.NewReader(file))
    if err != nil {
        fmt.Println("Error: failed to insert content ", err)
        return false, nil;
    }

    result := C.CString(cid)
    return true, result;
}

//export go_ipfs_cache_get
func go_ipfs_cache_get(c_file_obj_id *C.char, c_filename *C.char) bool {
	file_obj_id := C.GoString(c_file_obj_id)
	filename := C.GoString(c_filename)

    if debug {
        fmt.Println("go_ipfs_cache_get start");
        defer fmt.Println("go_ipfs_cache_get end");
    }

    reader, err := coreunix.Cat(g.ctx, g.node, file_obj_id)
    if err != nil {
        fmt.Println("go_ipfs_cache_get failed to Cat");
        return false
    }

    file,err := os.Create(filename)
    if err != nil {
        fmt.Println("Error: Create failed! ", err)
        return false
    }
    defer file.Close()

    _, err = io.Copy(file, reader)
    if err != nil {
        fmt.Println("Error: copy failed! ", err)
        return false
    }

    return true
}

//export go_ipfs_cache_cat
func go_ipfs_cache_cat(c_cid *C.char, fn unsafe.Pointer, fn_arg unsafe.Pointer) {
	cid := C.GoString(c_cid)

	go func() {
		if debug {
			fmt.Println("go_ipfs_cache_cat start");
			defer fmt.Println("go_ipfs_cache_cat end");
		}

		reader, err := coreunix.Cat(g.ctx, g.node, cid)

		if err != nil {
			fmt.Println("go_ipfs_cache_cat failed to Cat");
			C.execute_data_cb(fn, nil, C.size_t(0), fn_arg)
			return
		}

		bytes, err := ioutil.ReadAll(reader)
		if err != nil {
			fmt.Println("go_ipfs_cache_cat failed to read");
			C.execute_data_cb(fn, nil, C.size_t(0), fn_arg)
			return
		}

		cdata := C.CBytes(bytes)
		defer C.free(cdata)

		C.execute_data_cb(fn, cdata, C.size_t(len(bytes)), fn_arg)
	}()
}

type LsLink struct {
	Name, Hash string
	Size       uint64
	Type       unixfspb.Data_DataType
}

type LsObject struct {
	Hash  string
	Links []LsLink
}

//export go_ipfs_cache_ls
func go_ipfs_cache_ls(c_fpath *C.char) bool {
	fpath := C.GoString(c_fpath)

    if !g.node.OnlineMode() {
        return false
    }

    dserv := g.node.DAG

    p, err := path.ParsePath(fpath)
    if err != nil {
        return false
    }

    r := &path.Resolver{
        DAG:         g.node.DAG,
        ResolveOnce: uio.ResolveUnixfsOnce,
    }

    dagnode, err := core.Resolve(g.ctx, g.node.Namesys, r, p)
    if err != nil {
        return false
    }

    dir, err := uio.NewDirectoryFromNode(g.node.DAG, dagnode)
    if err != nil && err != uio.ErrNotADir {
        return false
    }

    var links []*node.Link
    if dir == nil {
        links = dagnode.Links()
    } else {
        links, err = dir.Links(g.ctx)
        if err != nil {
            return false
        }
    }

    output := make([]LsObject, 1)

    output[0] = LsObject{
        Hash:  fpath,
        Links: make([]LsLink, len(links)),
    }

    for j, link := range links {
        t := unixfspb.Data_DataType(-1)

        linkNode, err := link.GetNode(g.ctx, dserv)
        if err == merkledag.ErrNotFound {    // && !resolve
            // not an error
            linkNode = nil
        } else if err != nil {
            return false
        }

        if pn, ok := linkNode.(*merkledag.ProtoNode); ok {
            d, err := unixfs.FromBytes(pn.Data())
            if err != nil {
                return false
            }

            t = d.GetType()
        }

        output[0].Links[j] = LsLink{
            Name: link.Name,
            Hash: link.Cid.String(),
            Size: link.Size,
            Type: t,
        }
    }

    json_info, err := json.Marshal(output)

    fmt.Println("AA:", string(json_info));

    return true;
}

//export go_ipfs_cache_pin_add
func go_ipfs_cache_pin_add(c_cid *C.char, recursive bool) bool {
	cid := C.GoString(c_cid)

    paths := make([]string, 1)
    paths[0] = cid

    _, err := corerepo.Pin(g.node, g.ctx, paths, recursive);
	if err != nil {
		fmt.Println("err", err);
		return false
	}
    return true;
}

//export go_ipfs_cache_pin_rem
func go_ipfs_cache_pin_rem(c_cid *C.char, recursive bool) bool {
	cid := C.GoString(c_cid)

    paths := make([]string, 1)
    paths[0] = cid

    _, err := corerepo.Unpin(g.node, g.ctx, paths, recursive);
	if err != nil {
		fmt.Println("err", err);
		return false
	}

    return true;
}

//export go_ipfs_cache_bitswap_ledger
func go_ipfs_cache_bitswap_ledger(c_peer_id *C.char) (bool, *C.char) {
	peer_id := C.GoString(c_peer_id)

    bs, ok := g.node.Exchange.(*bitswap.Bitswap)
    if !ok {

        return false, nil
    }

    partner, err := peer.IDB58Decode(peer_id)
    if err != nil {
        return false, nil
    }

    ledger := bs.LedgerForPeer(partner)
    pretty_ledger := ledger;
    pretty_ledger.Peer  = partner.Pretty();

    json_info, err := json.Marshal(pretty_ledger)

    result := C.CString(string(json_info))
    return true, result
}

//export go_ipfs_cache_test
func go_ipfs_cache_test(c_cid *C.char) (bool, *C.char) {
//	cid := C.GoString(c_cid)

    test := make([]string, 5)

    test[0] = "a"
    test[1] = "b"
    test[2] = "c"


    testB, _ := json.Marshal(test)

    fmt.Println(string(testB))

    result := C.CString(string(testB))

    return true, result;
}




