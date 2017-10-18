//
// Created by Milan Franc on 9/22/17.
//

#include <stdio.h>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>

#include "ipfs_bindings.h"

int main(int argc, char* argv[])
{
    bool ret;
    std::string repo_path("/Users/milanfranc/.ipfs");

    try {

        ret = go_ipfs_cache_start((char*) repo_path.data() );
        std::cout << "daemon started\n";
        //std::this_thread::sleep_for(std::chrono::nanoseconds(100000000000));
        if (!ret) {
            throw std::runtime_error("Backend: Failed to start IPFS");
        }

        //NOTE: wait for start daemon...
        std::this_thread::sleep_for(std::chrono::nanoseconds(5000000000));

        std::string filename("/Users/milanfranc/test/foo");
        go_ipfs_cache_file_add_return ret_val = go_ipfs_cache_file_add((char*) filename.data());
        if (! (bool)ret_val.r0) {
            throw std::runtime_error("add: Failed..");
        }

        std::cout << "Res:" << std::string(ret_val.r1) << std::endl;
        free(ret_val.r1);

        char* peer_id = go_ipfs_cache_id();
        if (peer_id == NULL) {
            throw std::runtime_error("go_ipfs_cache_id: Failed..");
        }
        std::string my_peer_id(peer_id); free(peer_id);

        std::cout << "ID:" << my_peer_id << std::endl;

        go_ipfs_cache_bitswap_ledger_return ledger;
        ledger = go_ipfs_cache_bitswap_ledger((char*)my_peer_id.data());
        if (! (bool)ledger.r0) {
            throw std::runtime_error("go_ipfs_cache_bitswap_ledger: Failed..");
        }

        std::cout << "data:" << std::string(ledger.r1) << std::endl;


        std::string pin_hash("QmXQt6tBJkMVpmiJH57z7gd4GBGTUfCtaygiMKVbDsBmnV");
        ret = go_ipfs_cache_pin_add((char*) pin_hash.data(), true);
        if (!ret) {
            throw std::runtime_error("pin add: Failed..");
        }

        ret = go_ipfs_cache_pin_rem((char*) pin_hash.data(), true);
        if (!ret) {
            throw std::runtime_error("pin rem: Failed..");
        }

        ret = go_ipfs_cache_get((char*) pin_hash.data(), (char*) "/Users/milanfranc/test/abc");
        if (!ret) {
            throw std::runtime_error("get: Failed..");
        }

        pin_hash = "QmRewxxZybFMwx6GphFusSEnwSM5vM225iAJHCpCnx2kzK";
        ret = go_ipfs_cache_ls((char*) pin_hash.data());
        if (!ret) {
            throw std::runtime_error("ls: Failed..");
        }



//        std::string ls_data("home");
//        ret = go_ipfs_cache_ls((char*) ls_data.data());
//        if (!ret) {
//            throw std::runtime_error("ls: Failed..");
//        }


        std::cout << "hellooo\n";
        go_ipfs_cache_stop();
    }
    catch(std::exception& ex) {
        std::cout << "Error: " << ex.what() << std::endl;

    }

    return 0;
}