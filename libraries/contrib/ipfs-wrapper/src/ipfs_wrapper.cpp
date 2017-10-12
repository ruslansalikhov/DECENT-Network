
#include "ipfs_wrapper.hpp"
#include <ipfs_bindings.h>
#include <cstdlib>

#include <graphene/utilities/dirhelper.hpp>

IpfsWrapper::IpfsWrapper() : m_started(false)
{
}

IpfsWrapper::~IpfsWrapper()
{
    if (m_started) {
        go_ipfs_cache_stop();
    }
}

bool IpfsWrapper::Initialize(const char* repo_path)
{
    if (m_started) {
        go_ipfs_cache_stop();
        m_started = false;

        //NOTE: maybe some sleep...
    }

    std::string user_repo_path;
    if (repo_path == nullptr) {
        user_repo_path = graphene::utilities::decent_path_finder::instance().get_user_home().string();
        user_repo_path += "/.ipfs";

        repo_path = user_repo_path.c_str();
    }

    bool ret = go_ipfs_cache_start(const_cast<char*>(repo_path) );
    if (!ret) {
        return false;
    }

    //NOTE: wait for start daemon ?
    m_started = true;
    return true;
}

std::string IpfsWrapper::ipfs_id()
{
    char* peer_id = go_ipfs_cache_id();
    if (peer_id == nullptr) {
        return std::string();
    }

    std::string result(peer_id); free(peer_id);
    return result;
}

bool IpfsWrapper::ipfs_file_add(const std::string& filename, const std::string& ipfs_url, std::string& cid)
{
    go_ipfs_cache_file_add_wrapped_return ret_val;
    ret_val = go_ipfs_cache_file_add_wrapped(const_cast<char*>(filename.c_str()), const_cast<char*>(ipfs_url.c_str()) );
    if (! (bool)ret_val.r0) {
        return false;
    }

    cid = std::string(ret_val.r1); free(ret_val.r1);
    return true;
}

bool IpfsWrapper::ipfs_files_add_wrapped(const std::string& json_param, std::string& cid)
{
    go_ipfs_cache_add_files_wrapped_return ret_val;
    ret_val = go_ipfs_cache_add_files_wrapped(const_cast<char*>(json_param.c_str()) );
    if (! (bool)ret_val.r0) {
        return false;
    }

    cid = std::string(ret_val.r1); free(ret_val.r1);
    return true;
}

bool IpfsWrapper::ipfs_ls(const std::string& cid, std::string& json_info)
{
    go_ipfs_cache_ls_return ret;
    ret = go_ipfs_cache_ls(const_cast<char*>(cid.c_str()) );
    if (! (bool)ret.r0) {
        return false;
    }

    json_info = std::string(ret.r1); free(ret.r1);
    return true;
}

bool IpfsWrapper::ipfs_get(const std::string& cid, const std::string& filename)
{
    return go_ipfs_cache_get(const_cast<char*>(cid.c_str()), const_cast<char*>(filename.c_str()) );
}

bool IpfsWrapper::ipfs_pin_add(const std::string& cid, bool recursive)
{
    return go_ipfs_cache_pin_add(const_cast<char*>(cid.c_str()), recursive);
}

bool IpfsWrapper::ipfs_pin_rem(const std::string& cid, bool recursive)
{
    return go_ipfs_cache_pin_rem(const_cast<char*>(cid.c_str()), recursive);
}

bool IpfsWrapper::ipfs_bitswap_ledger(const std::string& peer_id, std::string& json_info)
{
    go_ipfs_cache_bitswap_ledger_return ledger;
    ledger = go_ipfs_cache_bitswap_ledger(const_cast<char*>(peer_id.c_str()));
    if (! (bool)ledger.r0) {
        return false;
    }

    json_info = std::string(ledger.r1); free(ledger.r1);
    return true;
}

