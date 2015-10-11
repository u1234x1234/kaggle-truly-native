import sys
import numpy as np
import scipy
import pandas as pd
import gc
sys.path.append('/home/linux12341234/xgboost/wrapper')
import xgboost as xgb

def logit(p):
    return np.log(p) - np.log(1 - p)

def inv_logit(p):
    return np.exp(p) / (1 + np.exp(p))

y1 = pd.read_csv('a1500_350.csv')#v7
y2 = pd.read_csv('res4.csv')#v4
y3 = pd.read_csv('res5.csv')#v5
y4 = pd.read_csv('space500.csv')#v6
y5 = pd.read_csv('joint_500_500.csv')#v8
y6 = pd.read_csv('joint_v2.csv')#v9
y7 = pd.read_csv('joint_v4_t.csv')#v10
y8 = pd.read_csv('joint_v5.csv')#v11
y9 = pd.read_csv('joint_v101.csv')#v101
y10 = pd.read_csv('joint_v102.csv')#v102
y11 = pd.read_csv('joint_v106.csv')#v106
y12 = pd.read_csv('joint_v109_r.csv')#v109
y13 = pd.read_csv('joint_v111_r001.csv')#v111
y14 = pd.read_csv('joint_v115_r002.csv')#v115
y15 = pd.read_csv('joint_v116.csv')#v116

a1 = pd.read_csv('mixPunkt_1234.csv')#saurabh
a2 = pd.read_csv('mixBlend_34Bank.csv')#saurabh

y1['sponsored'] = (y11['sponsored'] + y10['sponsored'] + y9['sponsored'] + y4['sponsored'] + y1['sponsored'] + y5['sponsored'] + y8['sponsored'] + y7['sponsored']) / 8
y1['sponsored'] = (y15['sponsored'] + y14['sponsored'] + y13['sponsored'] + y1['sponsored'] + y12['sponsored']) / 5

#y1.to_csv('sub_3.csv', index=False)
y1['sponsored'] = inv_logit(logit(a1['sponsored'].values) * 0.4 + logit(y1['sponsored'].values )* 0.3 + 
logit(y13['sponsored'].values )* 0.4)

y1['sponsored'] = (y1['sponsored']*0.8 + 0.2*a2['sponsored'])
y1.to_csv('blend.csv', index=False)
#print y1['sponsored']


#a1 = pd.read_csv('mixPunkt_1234.csv')['sponsored']#v7
#a2 = pd.read_csv('mixPunkt_1234_blend.csv')['sponsored']#v4
#a3 = pd.read_csv('blend.csv')['sponsored']#v5

#print ((a1 + y1['sponsored']) / 2)[0:15]
#print (a3)[0:15]

#print (np.sqrt(a1*a3))[0:15]
#print (a2)[0:15]

#print a3['sponsored']

#x = pd.read_csv('train_v2.csv')
#print sum(y1['sponsored'] > 0.5) / 66772.
#y1['sponsored'] = 1*(y1['sponsored'] > 0.5)
#y1.to_csv('test_pred.csv', index=False)

#print sum(x['sponsored'][:300000] == 1)
#print sum(x['sponsored'][:300000] == 0)

#print sum(x['sponsored'][300000:] == 1)
#print sum(x['sponsored'][300000:] == 0)