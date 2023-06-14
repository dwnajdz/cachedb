#include "cachelib/allocator/CacheAllocator.h"
#include "folly/init/Init.h"

namespace facebook {
namespace cachelib {

using Cache = cachelib::LruAllocator;
using CacheConfig = typename Cache::Config;
using CacheKey = typename Cache::Key;
using CacheReadHandle = typename Cache::ReadHandle;

std::unique_ptr<Cache> globalCache_;
cachelib::PoolId defaultPool_;

// initialize cache with config
// cache name
// size is in MB 1 = 1MB, 1000 = 1000MB = 1 GB
// bu
void initializeCache() {
  CacheConfig config;

  // set config variables
  config.setCacheName("cachedb_node1")
      .setCacheSize(1 * 1024 * 1024 * 1024)
      .setAccessConfig({25, 10});
  // will throw if bad config
  config.validate();

  globalCache_ = std::make_unique<Cache>(config);
  defaultPool_ = globalCache_->addPool(
      "default", globalCache_->getCacheMemoryStats().ramCacheSize);
}

void destroyCache() { globalCache_.reset(); }

CacheReadHandle get(CacheKey key) { return globalCache_->find(key); }

bool put(CacheKey key, const std::string& value) {
  auto handle = globalCache_->allocate(defaultPool_, key, value.size());
  if (!handle) {
    // cache may fail to evict due to too many pending writes
    return false;
  }

  std::memcpy(handle->getMemory(), value.data(), value.size());
  globalCache_->insertOrReplace(handle);

  return true;
}

// end of namespaces
} // namespace cachelib
} // namespace facebook

using namespace facebook::cachelib;

int main(int argc, char** argv) {
  folly::init(&argc, &argv);

  // 500 MB,
  // initializeCache("node_1", 500, 25, 10);
  initializeCache();

  std::string io_key;
  std::string io_value;

  while (true) {
    std::cout << "key: ";
    std::getline(std::cin, io_key);

    std::cout << "value: ";
    std::getline(std::cin, io_value);

    auto res = put(io_key, io_value);
    auto item = get(io_key);

    folly::StringPiece sp{reinterpret_cast<const char*>(item->getMemory()),
                          item->getSize()};
    std::cout << "key: " << io_key <<" value: " << sp;
  }

  destroyCache();
}