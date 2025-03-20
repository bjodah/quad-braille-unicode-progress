import os
import sys
import subprocess
from . import render_braille_progress
from .util import cpu_ram_nvidia

# def main(p1=100, p2=100, p3=100, p4=100):
#     s = render_braille_progress(p1,p2,p3,p4)
#     print(s)

def _percent(arg: str) -> int:
    arg = arg.strip()
    if arg.endswith('%'):
        ival = int(round(float(arg[:-1])))
    elif '.' or 'e' in arg:
        ival = int(round(float(arg)))
    else:
        ival = int(arg)
    return ival

def render4(p1, p2, p3, p4):
    return render_braille_progress(*map(_percent, [p1,p2,p3,p4]))


if __name__ == '__main__':
    import argh
    argh.dispatch_commands([render4, cpu_ram_nvidia])
    sys.exit(os.EX_OK)
