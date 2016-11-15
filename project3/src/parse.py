#!/usr/bin/python

import os

_dir = "gainestown_pinball_w0_d1B"

project_dir = "/home/gciotto/MO601-117136/project3/src/" + _dir + "/"
benchmarks = ["400.perlbench", "401.bzip2", "403.gcc", "429.mcf", "445.gobmk", "456.hmmer", "458.sjeng", "462.libquantum", "464.h264ref", "471.omnetpp", "473.astar", "483.xalancbmk", "410.bwaves", "416.gamess", "433.milc", "434.zeusmp", "436.cactusADM", "437.leslie3d", "444.namd", "447.dealII", "450.soplex", "453.povray", "454.calculix", "459.GemsFDTD", "465.tonto", "470.lbm", "481.wrf", "482.sphinx3"]
#benchmarks = ["400.perlbench"]
suffix = ""

sniper_log = "sim.out"

results = '/home/gciotto/MO601-117136/project3/'+ _dir + '.csv'

dict = {}

if __name__ == '__main__' :

    f_results = open(results, 'w')

    for benchmark in benchmarks:
        #print project_dir + benchmark

        if not benchmark in dict:
            dict[benchmark] = {}

        print project_dir + benchmark + suffix

        for (dirpath, dirnames, filenames) in os.walk(project_dir + benchmark + suffix):

            for dirname in dirnames:

                print dirname
    
                try:
                    f = open(project_dir + benchmark + suffix + "/" + dirname + "/" + sniper_log, 'r')
                except:
                    print "Erro no benchmark " + dirname
                    continue

                for line in f.readlines() :

                    if not "IPC" in line : continue

                    line =  line.strip().split('|')

                    if not dirname in dict[benchmark]:
                        dict[benchmark][dirname] = {}
                        dict[benchmark][dirname]["IPC"] = 0
                        dict[benchmark][dirname]["NAME"] = dirname


                    dict[benchmark][dirname]["IPC"] = float(line[1])

                    break

                f.close()

                f_results.write("%s,%f,\n" % (dict[benchmark][dirname]["NAME"], dict[benchmark][dirname]["IPC"]))

                
        #print benchmark
        print dict[benchmark]

		
    f_results.close()

			
				
			
