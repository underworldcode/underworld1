import os, platform
from config import Package

class SedSIM(Package):

    def gen_envs(self, loc):
        for env in Package.gen_envs(self, loc):
            env.PrependUnique(LIBS=['sedsim'])
            yield env
