#/bin/sh
#
# Load some alias to compile and run with the docker mpicc/mpif90/mpirun
#

ulfm_image=abouteiller/mpi-ft-ulfm

case _$1 in
    _|_load)
        docker pull $ulfm_image
        function make {
            docker run --user $(id -u):$(id -g) --cap-drop=all --security-opt label:disabled -v $PWD:/sandbox $ulfm_image make $@
        }
        function ompi_info {
            docker run --user $(id -u):$(id -g) --cap-drop=all $ulfm_image ompi_info $@
        }
        function mpirun {
            docker run --user $(id -u):$(id -g) --cap-drop=all --security-opt label:disabled -v $PWD:/sandbox $ulfm_image mpirun --map-by :oversubscribe --mca btl tcp,self $@
        }
        function mpiexec {
            docker run --user $(id -u):$(id -g) --cap-drop=all --security-opt label:disabled -v $PWD:/sandbox $ulfm_image mpiexec --map-by :oversubscribe --mca btl tcp,self $@
        }
        function mpiexec+gdb {
            docker run --user $(id -u):$(id -g) --cap-drop=all --security-opt label:disabled -v $PWD:/sandbox --cap-add=SYS_PTRACE --security-opt seccomp=unconfined $ulfm_image mpiexec --map-by :oversubscribe --mca btl tcp,self $@
        }
        function mpicc {
            docker run --user $(id -u):$(id -g) --cap-drop=all --security-opt label:disabled -v $PWD:/sandbox $ulfm_image mpicc $@
        }
        function mpif90 {
            docker run --user $(id -u):$(id -g) --cap-drop=all --security-opt label:disabled -v $PWD:/sandbox $ulfm_image mpif90 $@
        }
        echo "#  Function alias set for 'make', 'mpirun', 'mpiexec', 'mpicc', 'mpif90'."
        echo "source dockervars.sh unload # remove these aliases."
        echo "#    These commands now run from the ULFM Docker image."
        echo "#    Use \`mpiexec --with-ft ulfm\` to turn ON fault tolerance."
        mpirun --version
        ;;
    _unload)
        unset -f make
        unset -f ompi_info
        unset -f mpirun
        unset -f mpiexec
        unset -f mpicc
        unset -f mpif90
        echo "#  Function alias unset for 'make', 'mpirun', 'mpiexec', 'mpicc', 'mpif90'."
        ;;
    *)
        echo "#  This script is designed to load aliases for for 'make', 'mpirun', 'mpiexec', 'mpicc', 'mpif90'."
        echo "#  After this script is sourced in your local shell, these commands would run from the ULFM Docker image."
        echo "source dockervars.sh load # alias make, mpirun, etc. in the current shell"
        echo "source dockervars.sh unload # remove aliases from the current shell"
esac

