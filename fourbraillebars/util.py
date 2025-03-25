import subprocess
from typing import NamedTuple
import xml.etree.ElementTree as ET

class TotalUsedFree(NamedTuple):
    total: int
    used: int
    free: int

class FreeMem_m(NamedTuple):
    mem: TotalUsedFree
    swap: TotalUsedFree

    @classmethod
    def from_call(cls) -> "FreeMem_m":
        cp = subprocess.run("free -m", shell=True, stdout=subprocess.PIPE)
        for ln in cp.stdout.decode('utf-8').splitlines():
            if ln.startswith('Mem:'):
                mem = TotalUsedFree(*map(int, ln.split()[1:4]))
            if ln.startswith('Swap:'):
                swp = TotalUsedFree(*map(int, ln.split()[1:4]))
        return cls(mem, swp)

class CPU_usage(NamedTuple):
    user: float
    system: float
    nice: float
    idle: float
    wait_io: float
    hi: float  # hardware interrupts
    si: float  # software interrupts
    st: float  # steal vm

    @classmethod
    def from_call(cls) -> "CPU_usage":
        cp = subprocess.run('top -b -n1', shell=True, stdout=subprocess.PIPE)
        for ln in cp.stdout.decode('utf-8').splitlines():
            if ln.startswith('%Cpu(s):'):
                _, parts = ln.split(':')
                vals = {k: v for v, k in map(str.split, parts.split(','))}
                return cls(*[float(vals[fld[:2]]) for fld in cls._fields])
                # return cls(user=vals['us'], system=vals['sy'], nice=vals['ni'], idle=vals['id'],
                #            wait_io=vals['wa'], hw_irq=vals['hi'], sw_irq=vals['si'], steal_vm=vals['st'])


class NvidiaGpuUsage(NamedTuple):
    fan_percent: float  # 0-100
    mem_use: float  # VRAM GiB
    mem_tot: float  # VRAM GiB
    gpu_percent: float  # 0-100 "utlitzation"
    pwr: float  # watts
    temp: float  # celsius

    @classmethod
    def from_call(cls) -> "NvidiaGpuUsage":
        cp = subprocess.run('nvidia-smi -q -x'.split(), stdout=subprocess.PIPE)
        root = ET.fromstring(cp.stdout.decode('utf-8'))
        gpu = root.find("gpu")
        fan_speed = gpu.find("fan_speed").text.strip()[:-2]
        mem_use_mib = gpu.find("fb_memory_usage/used").text.strip()[:-4]
        mem_tot_mib = gpu.find("fb_memory_usage/total").text.strip()[:-4]
        gpu_util = gpu.find("utilization/gpu_util").text.strip()[:-2]
        power_draw = gpu.find("gpu_power_readings/power_draw").text.strip()[:-2]
        temp = gpu.find("temperature/gpu_temp").text.strip()[:-2]

        return cls(fan_percent=float(fan_speed), mem_use=float(mem_use_mib) / 1024,
                   mem_tot=float(mem_tot_mib) / 1024, gpu_percent=float(gpu_util),
                   pwr=float(power_draw), temp=float(temp))



def cpu_ram_nvidia():
    free_m = FreeMem_m.from_call()
    cpu = CPU_usage.from_call()
    gpu = NvidiaGpuUsage.from_call()
    return cpu.user, free_m.mem.used/free_m.mem.total*100, gpu.gpu_percent, gpu.mem_use/gpu.mem_tot
