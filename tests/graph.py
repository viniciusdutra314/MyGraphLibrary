import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import scipy as sci
import numpy as np
from scipy.optimize import curve_fit

df=pd.read_csv("performance_comparison_igraph.csv")
df['V']= df['graph_file'].apply(lambda x: int(x.split('_')[1]))
df = df.rename(columns={'igraph_time_ms': 'igraph_time_s', 'my_code_time_ms': 'my_code_time_s'})
df['igraph_time_s'] /= 1000
df['my_code_time_s'] /= 1000
df['E']= df['graph_file'].apply(lambda x: int(x.split('_')[2]))*df['V'] 

# --- 1. Your specific model: a*E*V + b*log(V)*V^2 ---
def time_model(X, a, b):
    V, E = X
    # Use V_safe to avoid log(1) = 0 if V=1 (though your data is > 1)
    V_safe = np.maximum(V, 1.0001) 
    return a * E * V + b * np.log(V_safe) * V**2

# --- 2. Log-transformed model for fitting ---
# We fit log(prediction) vs log(actual_time)
# This minimizes the relative error.
def log_time_model_fit(X, a, b):
    # Get the model's prediction
    model_pred = time_model(X, a, b)
    # Add a tiny floor (1e-30) to avoid log(0) if the model predicts 0
    return np.log(np.maximum(model_pred, 1e-30))

# --- 3. Perform fit in log-space ---
V_data = df['V'].values
E_data = df['E'].values
# We fit against the log of the actual time data
log_time_data = np.log(df['my_code_time_s'].values) 
X_data = np.vstack((V_data, E_data))

# Provide initial guesses for a and b (e.g., small positive numbers)
initial_guesses = [1e-10, 1e-10]
# Provide bounds to ensure a and b are non-negative
param_bounds = ([0, 0], [np.inf, np.inf])

# Fit the log-transformed model
# This finds the 'a' and 'b' that make log_time_model_fit(X, a, b)
# as close as possible to log_time_data
popt, pcov = curve_fit(log_time_model_fit, X_data, log_time_data, p0=initial_guesses, bounds=param_bounds)

# 'popt' now contains the fitted [a_fit, b_fit]
a_fit, b_fit = popt
print(f"Fitted parameter 'a': {a_fit}")
print(f"Fitted parameter 'b': {b_fit}")

# --- 4. Plotting ---
sns.set_theme(style="whitegrid")
fig,axis=plt.subplots(ncols=3,figsize=(15,6))
scatter_igraph=sns.scatterplot(data=df, x="V", y="igraph_time_s", hue="E", s=100, ax=axis[0])
scatter_my_code=sns.scatterplot(data=df, x="V", y="my_code_time_s", hue="E", s=100, ax=axis[1])
scatter_speedup=sns.scatterplot(data=df, x="V", y="speedup", hue="E", s=100, ax=axis[2])

handles, labels = axis[0].get_legend_handles_labels()
label_to_color = {lbl: h.get_gapcolor() for h, lbl in zip(handles[1:], labels[1:])} 

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
    
    # --- 5. Add fit visualization to 'My Code' plot ---
    if ax == axis[1]:
        v_plot = np.logspace(np.log10(df['V'].min()), np.log10(df['V'].max()), 50)
        
        for e_val_str, color in label_to_color.items():
            e_val = int(e_val_str)
            
            e_plot_data = np.full_like(v_plot, e_val)
            x_plot_data = np.vstack((v_plot, e_plot_data))
            
            # Calculate predicted time using the fitted 'a_fit' and 'b_fit'
            # with the original (non-log) time_model
            time_pred = time_model(x_plot_data, a_fit, b_fit) 
            
            ax.plot(v_plot, time_pred, color=color, linestyle='--')

scatter_igraph.set( title="Python-Igraph")
scatter_my_code.set(title="My Code")
scatter_speedup.set(title="Speedup")

fig.legend(handles, labels, title="E", loc='upper center', ncols=6)
fig.tight_layout(rect=[0, 0, 1, 0.90]) 
fig.savefig("benchmark_with_fit_v3.png") # New filename
print("Saved plot to 'benchmark_with_fit_v3.png'")
# plt.show()