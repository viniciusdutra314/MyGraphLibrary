import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import scipy as sci

df=pd.read_csv("performance_comparison_igraph.csv")
df['V']= df['graph_file'].apply(lambda x: int(x.split('_')[1]))
df = df.rename(columns={'igraph_time_ms': 'igraph_time_s', 'my_code_time_ms': 'my_code_time_s'})
df['igraph_time_s'] /= 1000
df['my_code_time_s'] /= 1000
df['E']= df['graph_file'].apply(lambda x: int(x.split('_')[2]))
sns.set_theme(style="whitegrid")
fig,axis=plt.subplots(ncols=3,figsize=(15,6))
scatter_igraph=sns.scatterplot(data=df, x="V", y="igraph_time_s", hue="E", s=100, ax=axis[0])
scatter_my_code=sns.scatterplot(data=df, x="V", y="my_code_time_s", hue="E", s=100, ax=axis[1])
scatter_speedup=sns.scatterplot(data=df, x="V", y="speedup", hue="E", s=100, ax=axis[2])
handles, labels = axis[0].get_legend_handles_labels()

for ax in axis:
    ax.set_xscale("log")
    if ax != axis[2]: 
        ax.set_yscale("log")
    ax.set_xlabel("Number of vertices (V)")
    
    if ax != axis[2]:
        ax.set_ylabel("Execution time (s)")
    else:
        ax.set_ylabel("Speedup (igraph time / my_code time)")
    
    if ax.get_legend() is not None:
        ax.get_legend().remove()

scatter_igraph.set( title="Python-Igraph")
scatter_my_code.set(title="My Code")
scatter_speedup.set(title="Speedup")

fig.legend(handles, labels, title="E", loc='upper center', ncols=6)
fig.tight_layout(rect=[0, 0, 1, 0.90]) 
fig.savefig("benchmark.png")
for row in df.itertuples():
    print(f"Graph: {row.graph_file},  efficiency: {row.efficiency}")    