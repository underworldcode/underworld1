import os
from config import Package
from PETSc import PETSc

class PETScExt(Package):

    def setup_dependencies(self):
        self.petsc = self.add_dependency(PETSc)

    def gen_locations(self):
        extloc = 'Solvers/PetscExt'
        extdir = os.path.join(os.getcwdu(), extloc)
        extinc = os.path.join(extdir, 'include')
        extlib = os.path.join(extdir, 'lib')  # if petscarch exists will be updated below
        yield (extdir,[extinc],[extlib])

    def gen_envs(self, loc):
        env = self.env.Clone()

        # force the petscext_lib_dir to be used
        # if it's explicitly defined
        if self.get_option('petscext_lib_dir'):
           loc[2][0] = self.get_option('petscext_lib_dir')
        # guess what path is applicable
        else:
           self.arch = self.petsc.arch
           if os.path.exists( loc[2][0] ):
             if self.arch is not None:
               loc[2][0] = os.path.join(loc[2][0], self.arch)

           else:
             if self.arch is not None:
               loc[2][0] = os.path.join(loc[0], self.arch, 'lib')

        env.AppendUnique(CPPPATH=loc[1])
        env.AppendUnique(LIBPATH=loc[2])
        env.AppendUnique(RPATH=loc[2])

        # In addition, export the PETSc base directory.
        if self.petsc.location[0]:
            env.AppendUnique(CPPPATH=[self.petsc.location[0]])

        # Try each library set.
        libs = ['petscext_utils', 'petscext_snes', 'petscext_ksp',
                'petscext_pc', 'petscext_mat', 'petscext_vec',
                'petscext_helpers']
        if self.find_libraries(loc[2], libs):
            lib_env = env.Clone()
            lib_env.PrependUnique(LIBS=libs)
            yield lib_env
        libs = ['petscext_utils', 'petscext_snes', 'petscext_ksp',
                'petscext_pc', 'petscext_mat', 'petscext_vec']
        if self.find_libraries(loc[2], libs):
            lib_env = env.Clone()
            lib_env.PrependUnique(LIBS=libs)
            yield lib_env
        libs = ['petscext']
        if self.find_libraries(loc[2], libs):
            lib_env = env.Clone()
            lib_env.PrependUnique(LIBS=libs)
            yield lib_env

    def check(self, conf, env):
        return conf.CheckLibWithHeader(None,
                                       ['mpi.h', 'petsc.h', 'petscvec.h', 'petscmat.h',
                                        'petscksp.h', 'petscsnes.h',
                                        'petscext.h', 'petscext_vec.h', 'petscext_mat.h',
                                        'petscext_ksp.h', 'petscext_snes.h'], 'c',
                                       autoadd=0)
