#!/usr/bin/python

import os

#_dir = "4KB_article_pinballs_w100M_d30M_03_12"
#_dir = "4MB_article_pinballs_w100M_d30M_03_12"
_dir = "ideal_article_pinballs_w100M_d30M_03_12"

project_dir = "/home/gciotto/MO601-117136/project4/src/" + _dir + "/"
all = ["400.perlbench", "401.bzip2", "403.gcc", "429.mcf", "445.gobmk", "456.hmmer", "458.sjeng", "462.libquantum", "464.h264ref", "471.omnetpp", "473.astar", "483.xalancbmk", "410.bwaves", "416.gamess", "433.milc", "434.zeusmp", "436.cactusADM", "437.leslie3d", "444.namd", "447.dealII", "450.soplex", "453.povray", "454.calculix", "459.GemsFDTD", "465.tonto", "470.lbm", "481.wrf", "482.sphinx3"]
#benchmarks = all[:12]
benchmarks = all
suffix = ""

print benchmarks

sniper_log = "sim.out"

results = '/home/gciotto/MO601-117136/project4/'+ _dir + '.csv'

d_result = {}

if __name__ == '__main__' :

    f_results = open(results, 'w')

    for benchmark in benchmarks:

        print project_dir + benchmark + suffix

        for dirname in os.listdir(project_dir + benchmark + suffix):

#            print dirname
            ref_entry = dirname[8:dirname.index("-ref")]
            
            print ref_entry

            d_result[ref_entry] = {}
            d_result[ref_entry]["NAME"] = ref_entry

            for dirname_pinball in os.listdir(project_dir + benchmark + suffix + "/" + dirname):

                begin = dirname_pinball.index("_0-") + 3
                end = dirname_pinball.index(".0") 

                region_ratio = float(dirname_pinball [begin : end]) / 1e5

#                print region_ratio

                try:
                    f = open(project_dir + benchmark + suffix + "/" + dirname + "/" + dirname_pinball +"/"+ sniper_log, 'r')
                except:
                    print " /!\\/!\\/!\\ Erro no benchmark " + dirname + "( " + str(region_ratio) + " ) /!\\/!\\/!\\"
                    if not "IPC" in d_result[ref_entry]:
                        d_result[ref_entry]["IPC"] = 0.0  
                    d_result[ref_entry]["IPC"] = d_result[ref_entry]["IPC"] * (1 + region_ratio)
                    continue

                lines = iter(f.readlines())

                for line in lines:

                    line =  line.strip().split('|')
                    line[0] = line[0].strip()
               
                    if line[0] == "IPC" or line[0] == "Cycles" or line[0] == "Instructions":

#                        if line[0] == "IPC":
#                            print line[0] + " = " + line[1] + " (" + str(region_ratio) + ")"

                        if not line[0] in d_result[ref_entry]:
                            d_result[ref_entry][line[0]] = 0.0    

                        d_result[ref_entry][line[0]] = d_result[ref_entry][line[0]] + float(line[1]) * region_ratio

                    elif line[0] == "I-TLB" or line[0] == "D-TLB":

                        if not line[0] in d_result[ref_entry]:
                           d_result[ref_entry][line[0]] = {}    

                        for i in range(2):

                            _line = next(lines)
                            _line =  _line.strip().split('|')
                            _line[0] = _line[0].strip()

                            if not _line[0] in d_result[ref_entry][line[0]]:
                                d_result[ref_entry][line[0]][_line[0]] = 0.0
   
                            d_result[ref_entry][line[0]][_line[0]] = d_result[ref_entry][line[0]][_line[0]] + float(_line[1]) * region_ratio
                        
                f.close()

#            print d_result[ref_entry]
#            print str((d_result[ref_entry]["Instructions"])/d_result[ref_entry]["Cycles"])

            f_results.write("%s,%f,\n" % (d_result[ref_entry]["NAME"], d_result[ref_entry]["IPC"]))

           
        #print d_result[ref_entry]

		
    f_results.close()

			
				
			
