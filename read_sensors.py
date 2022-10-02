import pandas
import matplotlib.pyplot as plt
import numpy as np
def temp_reject_outliers(data, m=2):
    data2 = data[data<-5]
    data2 = data2[data2>-50]
    return data2

listOfColumnNamesTemp=["dtime", "vA1","rA1","tA1","pA1", "vA2","rA2","tA2","pA2", "vB1","rB1","tB1","pB1", "vB2","rB2","tB2","pB2", "vC1","rC1","tC1","pC1", "vC2","rC2","tC2","pC2"]
listOfColumnNamesA=["dtime",       "iA1",      "iA2"]
listOfColumnNamesB=["dtime", "sB1","iB1","sB2","iB2"]

listOfMayRuns_52deg_Irad = [46040,46041,46043,46044,46046,46048,46049,46053,46054,46058,46055,46057,46056,46059,46086,46087,46088,46090,46091,46094,46097,46098,46099,46100,46101,46103]
listOfMayRuns_52deg_NonI = [46066,46068,46070,46071,46073,46075,46076]
listOfMayRuns_0deg_Irad = [46119,46122,46123,46126,46128,46130,46131,46133,46137,46138,46139,46141,46142,46143,46144,46146,46147,46148,46149,46179,46180,46181,46182,46183,46184,46185,46186]
listOfMayRuns_0deg_NonI = [46153,46172,46174,46170,46171,46168,46169,46166,46167,46159,46160]
listOfMayRuns = listOfMayRuns_52deg_Irad + listOfMayRuns_52deg_NonI + listOfMayRuns_0deg_Irad + listOfMayRuns_0deg_NonI

print( "RUN, || MEAN_tA1, STD_tA1 || MEAN_tA2, STD_tA2 || MEAN_tB1, STD_tB1 || MEAN_tB2, STD_tB2 ||| MEAN_iA1, STD_iA1 || MEAN_iA2, STD_iA2 || MEAN_iB1, STD_iB1 || MEAN_iB2, STD_iB2" )

for run in listOfMayRuns:
    mean_tA1 = -999
    mean_tA2 = -999
    mean_tB1 = -999
    mean_tB2 = -999
    std_tA1 = -999
    std_tA2 = -999
    std_tB1 = -999
    std_tB2 = -999
    mean_iA1 = -999
    mean_iA2 = -999
    mean_iB1 = -999
    mean_iB2 = -999
    std_iA1 = -999
    std_iA2 = -999
    std_iB1 = -999
    std_iB2 = -999
    try:
        df = pandas.read_csv("/eos/cms/store/group/dpg_mtd/comm_mtd/TB/MTDTB_FNAL_Apr2021/testing-4-18-2021/5-22/physics_data/sensors_data/feedback_{0}.csv".format(run),
                             skiprows=3,error_bad_lines=False,header=None,names=listOfColumnNamesTemp,lineterminator='\n')
        df.tA1 = pandas.to_numeric(df.tA1,errors='coerce')
        df.tA2 = pandas.to_numeric(df.tA2,errors='coerce')
        df.tB1 = pandas.to_numeric(df.tB1,errors='coerce')
        df.tB2 = pandas.to_numeric(df.tB2,errors='coerce')
        tA1 = temp_reject_outliers(df.tA1)
        tA2 = temp_reject_outliers(df.tA2)
        tB1 = temp_reject_outliers(df.tB1)
        tB2 = temp_reject_outliers(df.tB2)
        mean_tA1 = np.mean(tA1)
        mean_tA2 = np.mean(tA2)
        mean_tB1 = np.mean(tB1)
        mean_tB2 = np.mean(tB2)
        std_tA1 = np.std(tA1)
        std_tA2 = np.std(tA2)
        std_tB1 = np.std(tB1)
        std_tB2 = np.std(tB2)
    except:
        mean_tA1 = -9999
        mean_tA2 = -9999
        mean_tB1 = -9999
        mean_tB2 = -9999
        std_tA1 = -9999
        std_tA2 = -9999
        std_tB1 = -9999
        std_tB2 = -9999
    try:
        dfB = pandas.read_csv("/eos/cms/store/group/dpg_mtd/comm_mtd/TB/MTDTB_FNAL_Apr2021/testing-4-18-2021/5-22/physics_data/sensors_data/currentB1B2_{0}.csv".format(run),
                              sep='[:,]',skiprows=3,error_bad_lines=False,header=None,names=listOfColumnNamesB,lineterminator='\n',engine="python")
        dfB.iB1 = pandas.to_numeric(dfB.iB1,errors='coerce')
        dfB.iB2 = pandas.to_numeric(dfB.iB2,errors='coerce')
        mean_iB1 = 1000*np.mean(dfB.iB1)
        mean_iB2 = 1000*np.mean(dfB.iB2)
        std_iB1 = 1000*np.std(dfB.iB1)
        std_iB2 = 1000*np.std(dfB.iB2)
    except:
        mean_iB1 = -9999
        mean_iB2 = -9999
        std_iB1 = -9999
        std_iB2 = -9999
    try:
        dfA = pandas.read_csv("/eos/cms/store/group/dpg_mtd/comm_mtd/TB/MTDTB_FNAL_Apr2021/testing-4-18-2021/5-22/physics_data/sensors_data/sourcemeas_{0}.csv".format(run),
                              skiprows=3,error_bad_lines=False,header=None,names=listOfColumnNamesA,lineterminator='\n')
        dfA.iA1 = pandas.to_numeric(dfA.iA1,errors='coerce')
        dfA.iA2 = pandas.to_numeric(dfA.iA2,errors='coerce')
        mean_iA1 = 1000*np.mean(dfA.iA1)
        mean_iA2 = 1000*np.mean(dfA.iA2)
        std_iA1 = 1000*np.std(dfA.iA1)
        std_iA2 = 1000*np.std(dfA.iA2)
    except:
        mean_iA1 = -9999
        mean_iA2 = -9999
        std_iA1 = -9999
        std_iA2 = -9999
    print( "{0}, || {1:.2f}, {2:.2f}, || {3:.2f}, {4:.2f}, || {5:.2f}, {6:.2f}, || {7:.2f}, {8:.2f} ||| {9:.2f}, {10:.2f}, || {11:.2f}, {12:.2f}, || {13:.2f}, {14:.2f}, || {15:.2f}, {16:.2f}".format(run,mean_tA1,std_tA1,mean_tA2,std_tA2,mean_tB1,std_tB1,mean_tB2,std_tB2,mean_iA1,std_iA1,mean_iA2,std_iA2,mean_iB1,std_iB1,mean_iB2,std_iB2) )
    