<p align="center">
  <img width="500" height="140" alt="CacheLib" src="website/static/img/CacheLib-Logo-Large-transp.png">
</p>

# cachedb
CacheDB is fast scalable in-memory database using gRPC protocol.

For data storing it uses CacheLib.
CacheLib is a pluggable caching engine to build and scale high performance cache services. See
[www.cachelib.org](https://cachelib.org) for documentation and more information.

## Building and installation

CacheLib provides a build script which prepares and installs all
dependencies and prerequisites, then builds CacheLib.
The build script has been tested to work on CentOS 8,
Ubuntu 18.04, and Debian 10.

```sh
git clone https://github.com/facebook/CacheLib
cd CacheLib
./contrib/build.sh -d -j -v

# The resulting library and executables:
./opt/cachelib/bin/cachebench --help
```

Re-running `./contrib/build.sh` will update CacheLib and its dependencies
to their latest versions and rebuild them.

See [build](https://cachelib.org/docs/installation/) for more details about
the building and installation process.


## Contributing

We'd love to have your help in making CacheLib better. If you're interested,
please read our [guide to contributing](CONTRIBUTING.md)



## License

cachedb is *apache* licensed, as found in the [LICENSE](LICENSE) file.
