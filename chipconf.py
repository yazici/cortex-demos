import os

__all__ = []

def configure_arm(env):
    if 'ARM_TOOLCHAIN_PREFIX' in os.environ:
        arm_prefix = os.environ['ARM_TOOLCHAIN_PREFIX']
    else:
        arm_prefix = 'arm-none-eabi-'

    arm_tool = lambda t: arm_prefix + t

    env.Replace(
        AR=arm_tool('ar'),
        AS=arm_tool('as'),
        CC=arm_tool('gcc'),
        CXX=arm_tool('g++'),
        LINK=arm_tool('gcc'),
        GDB=arm_tool('gdb'),
        OBJCOPY=arm_tool('objcopy'),
        OBJDUMP=arm_tool('objdump'),
        SIZE=arm_tool('size'))

def configure_chip(env, chipname):
    arch_flags = []
    arch_defines = []
    if chipname.lower().startswith('nrf52'):
        configure_arm(env)
        arch_flags = [
            '-mthumb',
            '-mcpu=cortex-m4',
            '-mfloat-abi=hard',
            '-mfpu=fpv4-sp-d16',
            ]
    elif chipname.lower() == 'nativetest':
        arch_defines=['TEST_MEMIO']

    env.AppendUnique(CCFLAGS=arch_flags)
    env.AppendUnique(LINKFLAGS=arch_flags)
    env.AppendUnique(CPPDEFINES=arch_defines)