#
#
llvm-profdata merge -o default.profdata -sparse=true default.profraw
llvm-cov export ./build/fftplayer --instr-profile default.profdata >perfcov.json
python3 format.py
gprof ./build/fftplayer gmon.out >report.txt
