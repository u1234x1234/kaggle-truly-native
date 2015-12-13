import sys
import time
import numpy as np
import scipy
import pandas as pd
import sklearn
sys.path.append('/root/native/xgboost/wrapper')
import xgboost as xgb
from sklearn import metrics

y = pd.read_csv('train_v2.csv').values[:, 1].astype(np.float32)
#dtrain = xgb.DMatrix('sep_joint_train5.dmat', label = y[:300000])
dval = xgb.DMatrix('sep_joint_val5.dmat', label = y[300000:])

param = {'booster':'gbtree',
     'max_depth':30,
     'min_child_weight':0.1,
     'eta':0.03,
#     'gamma':4.0,
     'silent':1,
#      'scale_pos_weight':ratio,
     'objective':'binary:logistic',
#     'objective':'rank:pairwise',
     'lambda':1.0,
     'alpha':1.0,
#      'lambda_bias':0.5,
     'nthread':12,
#      'max_delta_step': 1,
#     'num_class':2,
     #'colsample_bytree':1,
     'subsample':0.9,
      'colsample_bytree': 0.9,
     'eval_metric':'auc'
     }
num_round = 1
#watchlist  = [(dtrain,'train')]
print ('training..')

small = np.fromfile('small', dtype=np.float32)
print small.shape

bst = xgb.Booster(param, [dval], model_file='model_joint_v125')
for i in range(num_round):
#    tm = time.time()
#    bst.update(dtrain, i)
    preds = bst.predict(dval)
#    preds = preds * (small * 0.01 + 1)
    preds = preds + 0.005 * small
    print (metrics.roc_auc_score(y[300000:], preds))
#    if i % 5 == 0:
#        bst.save_model('model_joint_v125')
#    print i
#    print ('elapsed time: ', time.time() - tm)
#    if i == 200:
#        param['eta'] = 0.02
#        bst.set_param(param)

    
    
    