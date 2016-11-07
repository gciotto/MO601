#!/usr/bin/python

import os

project_dir = "/home/gciotto/MO601-117136/project3/src/"
#benchmarks = ["410.bwaves", "429.mcf", "434.zeusmp", "436.cactusADM", "459.GemsFDTD", "400.perlbench", "401.bzip2", "481.wrf", "433.milc", "403.gcc", "toy"]
benchmarks = ["400.perlbench"]
suffix = ""

sniper_log = "sim.out"

results = '/home/gciotto/MO601-117136/project3/results_gainestown.csv'

dict = {}

if __name__ == '__main__' :

    f_results = open(results, 'w')

    for benchmark in benchmarks:
        print project_dir + benchmark

        if not benchmark in dict:
            dict[benchmark] = {}

        for (dirpath, dirnames, filenames) in os.walk(project_dir + benchmark):

            for dirname in dirnames:

                if suffix in dirname:
	
                    print dirname

                    b_ratio = dirname.index("_0-") + 3
                    e_ratio = dirname.index(".0") 

                    region_ratio = float(dirname [b_ratio : e_ratio]) / 1e5

                    b_entry = 8
                    e_ratio = dirname.index("-ref")

                    key = dirname [b_entry : e_ratio]

                    f = open(project_dir + benchmark + "/" + dirname + "/" + sniper_log,'r')

                    for line in f.readlines() :

                        if not "IPC" in line : continue

                        line =  line.strip().split('|')

                        if not key in dict[benchmark]:
                            dict[benchmark][key] = {}
                            dict[benchmark][key]["IPC"] = 0
                            dict[benchmark][key]["NAME"] = key


	                    dict[benchmark][key]["IPC"] = dict[benchmark][key]["IPC"] + float(line[1]) * region_ratio

                        break

                    f.close()

                
            print benchmark
            print dict[benchmark]
	            

		# f_KB.write("%d,%d,%d," % ((im["TotalMisses"] + 3*itlb_4KB["TotalMisses"]), itlb_4KB["TotalMisses"], 3*itlb_4KB["TotalMisses"]))
		# f_KB.write("%d,%d,%d,\n" % ((dm["TotalMisses"] + 3*dtlb_4KB["TotalMisses"]), dtlb_4KB["TotalMisses"], 3*dtlb_4KB["TotalMisses"]))

		
    f_results.close()

			
				
			
