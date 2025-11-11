import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
import os
import time

df = pd.read_csv("parallel_implementation.csv")
df['graph_file'] = df['graph_file'].apply(lambda x: os.path.basename(x))
df_omp = df[['cpu_cores', 'schedule', 'my_code_time_s']].copy()
df_omp.rename(columns={'my_code_time_s': 'time_s'}, inplace=True)

df_c11 = df[['cpu_cores', 'c11_threads_time_s']].copy()
df_c11.rename(columns={'c11_threads_time_s': 'time_s'}, inplace=True)
df_c11['schedule'] = 'C11_threads'

df_c11 = df_c11.drop_duplicates()
df_combined = pd.concat([df_omp, df_c11])
df_combined = df_combined.dropna(subset=['time_s'])


sns.set_theme(style="whitegrid")

print(df_combined)

ax = sns.pointplot(data=df_combined, 
                   x="cpu_cores", 
                   y="time_s", 
                   hue="schedule",
                   capsize=0.1, 
                   alpha=0.7)

ax.set_yscale('log')
ax.grid(True, which='both')
ax.set_xlabel("Number of CPU Cores", fontsize=12)
ax.set_ylabel("Execution Time (s)", fontsize=12)
plt.tight_layout()

plt.savefig("parallel_performance.png")