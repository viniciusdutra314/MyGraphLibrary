import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
import os
import time

df = pd.read_csv("parallel_implementation.csv")
df['graph_file'] = df['graph_file'].apply(lambda x: os.path.basename(x))

sns.set_theme(style="whitegrid")
ax=sns.pointplot(data=df, x="cpu_cores", y="my_code_time_s", hue="schedule", 
              capsize=0.1,alpha=0.5)

ax.set_yscale('log')
ax.grid(True, which='both')
plt.savefig("parallel_performance.png")