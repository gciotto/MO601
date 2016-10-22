#!/usr/bin/python

import os

project_dir = "/home/gciotto/MO601-117136/project2/src/"
benchmarks = ["410.bwaves", "429.mcf", "434.zeusmp", "436.cactusADM", "459.GemsFDTD", "400.perlbench", "401.bzip2", "481.wrf", "433.milc", "403.gcc", "toy"]
suffix = "22_10"

results_4KB = '/home/gciotto/MO601-117136/project2/results_4KB.csv'
results_4MB = '/home/gciotto/MO601-117136/project2/results_4MB.csv'

dict = {}

if __name__ == '__main__' :

	f_KB = open(results_4KB, 'w')
	f_MB = open(results_4MB, 'w')

	for benchmark in benchmarks:
		print project_dir + benchmark
		
		if not benchmark in dict:
			dict[benchmark] = {}
		
		for (dirpath, dirnames, filenames) in os.walk(project_dir + benchmark):
			for filename in filenames:

				if suffix in filename:
					
					print filename

					if benchmark == "toy" :
						region_ratio = 1.0
					else:
						begin = filename.index("_0-") + 3
						end = filename.index(".log") 

						region_ratio = float(filename [begin : end]) / 1e5

					f = open(project_dir + benchmark + "/" + filename,'r')

					for line in f.readlines() :
					
						if not ":" in line : continue
					
						line =  line.strip().split()


						if "TLB" in line[0] or line[0] in [ "Instruction_Memory:", "Data_memory:", "L1", "L2", "L3", "D1" ]:

							if ":" in line[0]:
								key = line[0][: line[0].index(":")]
							else: key = line[0]
						
							if not key in dict[benchmark]:
								dict[benchmark][key] = {}

							continue
					
						if line[0] in ['Store', 'Load', 'Total']:

							_access_type = line[1][:-1]

							if _access_type in ['Hits', 'Misses', 'Accesses'] :
						
								if not (line[0] + _access_type) in dict[benchmark][key]:
									dict[benchmark][key][line[0] + _access_type] = 0

								dict[benchmark][key][line[0] + _access_type] = dict[benchmark][key][line[0] + _access_type] + region_ratio * float (line[2])
					
		print benchmark
		#print dict[benchmark]		

		if benchmark == "toy":
			f_KB.write(benchmark + ',')
			f_MB.write(benchmark + ',')
		else:
			f_KB.write(benchmark[4:] + '.ref1,')
			f_MB.write(benchmark[4:] + '.ref1,')
	
		itlb_4KB = dict[benchmark]["ITLB_4K"]
		itlb_4MB = dict[benchmark]["ITLB_4M"]

		dtlb_4KB = dict[benchmark]["DTLB_4K"]
		dtlb_4MB = dict[benchmark]["DTLB_4M"]

		im = dict[benchmark]["Instruction_Memory"]
		dm = dict[benchmark]["Data_memory"]

		f_KB.write("%d,%d,%d," % ((im["TotalMisses"] + 3*itlb_4KB["TotalMisses"]), itlb_4KB["TotalMisses"], 3*itlb_4KB["TotalMisses"]))
		f_KB.write("%d,%d,%d,\n" % ((dm["TotalMisses"] + 3*dtlb_4KB["TotalMisses"]), dtlb_4KB["TotalMisses"], 3*dtlb_4KB["TotalMisses"]))

		f_MB.write("%d,%d,%d," % ((im["TotalMisses"] + 3*itlb_4MB["TotalMisses"]), itlb_4MB["TotalMisses"], 3*itlb_4MB["TotalMisses"]))
		f_MB.write("%d,%d,%d,\n" % ((dm["TotalMisses"] + 3*dtlb_4MB["TotalMisses"]), dtlb_4MB["TotalMisses"], 3*dtlb_4MB["TotalMisses"]))

		l1 = dict[benchmark]["L1"]
		l1_data = dict[benchmark]["D1"]
		l2 = dict[benchmark]["L2"]
		l3 = dict[benchmark]["L3"]

		print "L1 Instr: Hits = %d, Misses = %d, Total = %d" % (l1["TotalHits"], l1["TotalMisses"],l1["TotalAccesses"])
		print "L1 Data: Hits = %d, Misses = %d, Total = %d" % (l1_data["TotalHits"], l1_data["TotalMisses"],l1_data["TotalAccesses"])
		print "L2: Hits = %d, Misses = %d, Total = %d" % (l2["TotalHits"], l2["TotalMisses"],l2["TotalAccesses"])
		print "L3: Hits = %d, Misses = %d, Total = %d" % (l3["TotalHits"], l3["TotalMisses"],l3["TotalAccesses"])
		
	f_KB.close()
	f_MB.close()

			
				
			
