#!/bin/bash

#SBATCH --job-name Tonic_openmp
#SBATCH --error errors_%j.txt
#SBATCH --output output_%j.txt
#SBATCH --mail-user boldrincristian00@gmail.com
#SBATCH --mail-type END,FAIL
#SBATCH --partition allgroups
#SBATCH --ntasks 1
#SBATCH --cpus-per-task 8
#SBATCH --mem 16G
#SBATCH --time 00:02:00

cd $SLURM_SUBMIT_DIR

export OMP_NUM_THREADS=16

srun singularity exec ./container/Tonic_image.sif ../home/ParallelTCTN/code/build/Tonic @
