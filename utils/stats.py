import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.pyplot import figure

data = []
for i in range(2, 8):
    data.append(pd.read_csv('../resources/collisions/collisions' + str(i) + '.csv', sep=',')['collisions'])

xspc = np.linspace(0, 8191, 8192)

fig, axs = plt.subplots(3, 2)

axs[0][0].bar(xspc, data[0], color ='brown')
axs[0][1].bar(xspc, data[1], color ='green')
axs[1][0].bar(xspc, data[2], color ='green')
axs[1][1].bar(xspc, data[3], color ='brown')
axs[2][0].bar(xspc, data[4], color ='green')
axs[2][1].bar(xspc, data[5], color ='brown')

axs[0][0].set_title('first byte')
axs[0][1].set_title('length'    )
axs[1][0].set_title('sum'       )
axs[1][1].set_title('ROR'       )
axs[2][0].set_title('ROL'       )
axs[2][1].set_title('CRC32'     )

plt.gcf().set_size_inches(10, 10)

# plt.subplots_adjust(
#     left   =0.1,
#     bottom =0.1, 
#     right  =0.9, 
#     top    =0.9, 
#     wspace =0.4, 
#     hspace =0.4
# )

plt.savefig('../resources/colls.pdf')
