# Microsoft 3D Movie Maker

Released in 1995, this is the original source code to the Microsoft 3D Movie Maker project, now released
under the [MIT license](LICENSE) as open source.

## Building instructions

This project is unlikely to build successfully under modern hardware/software, but you can get started with compilation and get partial completed binaries. Here's what will get you going.
- Make sure this repo is checked out to a folder with a short name, ideally right on the root of a drive (i.e. C:\3d).
- You will need Visual C++ 2.0's dev tools (located under MSVC20\BIN on its installer disk) on your path. Modern compilers dislike some of the pre C++98 conventions.
- From the root of this repo, run ```setvars.bat``` you can change the values in this script to change what your build will target.
- Run nmake and you'll begin building 3d Movie Maker.

## Contributing

The source files in this repo are for historical reference and will be kept static, and this repository will
be archived. Feel free to fork this repo and experiment.

## Code cleanup

This code was restored from the Microsoft corporate archives and cleared for release.

- Developer names and aliases were removed, with the exception of current employees who worked on the
  original release who consented to keeping their names in place
- The archive consisted of several CDs, some of which were for alternate builds or products, and
  have been excluded
- The code does not build with today's engineering tools, and is released as-is.

## Trademarks

This project may contain trademarks or logos for projects, products, or services. Authorized use of Microsoft 
trademarks or logos is subject to and must follow 
[Microsoft's Trademark & Brand Guidelines](https://www.microsoft.com/en-us/legal/intellectualproperty/trademarks/usage/general).
Use of Microsoft trademarks or logos in modified versions of this project must not cause confusion or imply Microsoft sponsorship.
Any use of third-party trademarks or logos are subject to those third-party's policies.
