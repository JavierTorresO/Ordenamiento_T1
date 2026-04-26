import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os
import numpy as np

# Configuración estética profesional
sns.set_theme(style="whitegrid", palette="muted")
plt.rcParams['figure.figsize'] = [12, 8]
plt.rcParams['font.size'] = 12

def plot_execution_times():
    if not os.path.exists('reporte_escala_completa.csv'): return
    df = pd.read_csv('reporte_escala_completa.csv')
    
    plt.figure(figsize=(10, 6))
    ax = sns.lineplot(data=df, x='N_Size', y='Tiempo_s', hue='Algoritmo', marker='o', linewidth=2.5)
    ax.set_yscale('log') # Escala logarítmica para mejor visualización de tiempos
    ax.set_title('Tiempo de Ejecución vs Tamaño (Escala Logarítmica)', fontsize=15, pad=20)
    ax.set_ylabel('Tiempo (s) - Log Scale')
    ax.set_xlabel('Tamaño del Arreglo (N)')
    plt.tight_layout()
    plt.savefig('grafico_tiempos.png', dpi=300)
    print("Generado: grafico_tiempos.png")

def plot_speedup_efficiency():
    if not os.path.exists('tabla_speedup_eficiencia.csv'):
        print("Error: No se encuentra tabla_speedup_eficiencia.csv")
        return

    df = pd.read_csv('tabla_speedup_eficiencia.csv')
    
    # Asegurar orden correcto de N_Size en los gráficos
    n_order = ['2^20', '2^22', '2^24', '2^26']
    
    # --- 1. DASHBOARD DE SPEEDUP ---
    g = sns.relplot(
        data=df, x="Hilos", y="Speedup", hue="Algoritmo", col="N_Size",
        col_order=n_order, col_wrap=2, kind="line", marker="o", 
        height=4, aspect=1.2, linewidth=2
    )
    
    # Añadir línea de Speedup Ideal (y = x) en cada subgráfico
    for ax in g.axes.flat:
        ax.plot([1, 8], [1, 8], color='black', linestyle='--', alpha=0.7, label='Ideal')
        ax.set_xticks([1, 2, 4, 8])
        ax.set_ylim(0, 9)

    g.fig.subplots_adjust(top=0.9)
    g.fig.suptitle('Speedup por Tamaño de Arreglo (p = 1, 2, 4, 8)', fontsize=16)
    g.set_axis_labels("Hilos (p)", "Speedup (T1/Tp)")
    plt.savefig('grafico_speedup_facet.png', dpi=300)
    print("Generado: grafico_speedup_facet.png")

    # --- 2. DASHBOARD DE EFICIENCIA ---
    e = sns.relplot(
        data=df, x="Hilos", y="Eficiencia", hue="Algoritmo", col="N_Size",
        col_order=n_order, col_wrap=2, kind="line", marker="s", 
        height=4, aspect=1.2, linewidth=2
    )
    
    # Añadir línea de Eficiencia Ideal (y = 1) en cada subgráfico
    for ax in e.axes.flat:
        ax.axhline(y=1.0, color='red', linestyle='--', alpha=0.6, label='Ideal')
        ax.set_xticks([1, 2, 4, 8])
        ax.set_ylim(0, 1.2)

    e.fig.subplots_adjust(top=0.9)
    e.fig.suptitle('Eficiencia por Tamaño de Arreglo (p = 1, 2, 4, 8)', fontsize=16)
    e.set_axis_labels("Hilos (p)", "Eficiencia (S/p)")
    plt.savefig('grafico_eficiencia_facet.png', dpi=300)
    print("Generado: grafico_eficiencia_facet.png")

def plot_hardware_metrics():
    if not os.path.exists('reporte_escala_completa.csv'): return
    df = pd.read_csv('reporte_escala_completa.csv')
    
    # Cache Misses Ratio (Misses / References)
    df['Cache_Miss_Rate'] = (df['Cache_Misses'] / df['Cache_References']) * 100
    
    plt.figure(figsize=(10, 6))
    ax = sns.barplot(data=df, x='N_Size', y='Cache_Miss_Rate', hue='Algoritmo')
    ax.set_title('Tasa de Fallas de Caché (%)', fontsize=15)
    ax.set_ylabel('% Fallas / Referencias')
    plt.savefig('grafico_cache_rate.png', dpi=300)
    print("Generado: grafico_cache_rate.png")

    plt.figure(figsize=(10, 6))
    ax = sns.lineplot(data=df, x='N_Size', y='IPC', hue='Algoritmo', marker='p', linewidth=2)
    ax.set_title('Eficiencia de Ejecución (IPC)', fontsize=15)
    plt.savefig('grafico_ipc.png', dpi=300)
    print("Generado: grafico_ipc.png")

if __name__ == "__main__":
    print("🚀 Iniciando generación de Dashboard de alto rendimiento...")
    plot_execution_times()
    plot_speedup_efficiency()
    plot_hardware_metrics()
    print("✅ Dashboards guardados como archivos PNG de alta resolución.")
