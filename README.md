# Four Braille Bars Display

A utility to render system resource usage statistics as Braille progress bars.

## Features
- Displays four progress bars using Unicode Braille characters
- Monitors CPU usage, memory usage, and NVIDIA GPU status
- Command-line interface for quick system overview

## Usage
``python -m fourbraillebars [OPTIONS]``

## Installation
1. Clone repository
2. Install dependencies: `pip install .`

## Testing
Run tests with: `pytest`

## Contributing
- Open an issue for new features
- Submit pull requests for improvements
- Ensure tests pass

## C version
gpt-oss-120b wrote a C version in a few seconds. With minor tweak (avoid malloc of fixed size buffer),
it's checked in as `fourbraillebars.c`:
```shell-session
$ gcc -O2 -march=native -Wall -Wextra -o fourbbars fourbraillebars.c -lm
$ ./fourbbars 75 55 25 100
⣿⣿⣟⣛⣛⣋⣉⣁⣀⣀
$ hyperfine --warmup 1000 -N "./fourbbars 75 55 25 100"
... 351 µs
```

## License
MIT License - see [LICENSE](LICENSE)
