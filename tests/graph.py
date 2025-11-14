import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
import os
import time

df = pd.read_csv("parallel_implementation.csv")
sns.set_theme(style="whitegrid")


ax = sns.pointplot(data=df, 
                   x="cpu_cores", 
                   y="execution_time", 
                   hue="schedule",
                   capsize=0.1, 
                   alpha=0.7)
ax.set_yscale("log")
ax.grid(True, which='both')
ax.set_xlabel("Number of Cores", fontsize=14)
ax.set_ylabel("Execution Time (s)", fontsize=12)
plt.tight_layout()

plt.savefig("parallel_performance.png")