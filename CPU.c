import tkinter as tk
from tkinter import ttk, messagebox, filedialog
import pandas as pd
import numpy as np
from io import StringIO
from tkinter import font as tkfont
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

class CPUSchedulingApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Energy-Sufficient CPU Scheduling Simulator")
        self.root.geometry("1200x800")
        self.root.configure(bg="#f0f0f0")
        
        
        self.title_font = tkfont.Font(family="Helvetica", size=16, weight="bold")
        self.subtitle_font = tkfont.Font(family="Helvetica", size=12)
        self.button_font = tkfont.Font(family="Helvetica", size=10)
        
        
        self.style = ttk.Style()
        self.style.configure('TFrame', background="#f0f0f0")
        self.style.configure('TLabel', background="#f0f0f0", font=self.subtitle_font)
        self.style.configure('TButton', font=self.button_font, padding=6)
        self.style.configure('TCombobox', font=self.subtitle_font)
        self.style.configure('Treeview', font=('Consolas', 10), rowheight=25)
        self.style.configure('Treeview.Heading', font=('Helvetica', 10, 'bold'))
        
        self.dataset = None
        self.create_widgets()
    
    def create_widgets(self):
      
        header_frame = ttk.Frame(self.root, style='TFrame')
        header_frame.pack(fill=tk.X, padx=10, pady=10)
        
    
        title_label = ttk.Label(header_frame, 
                              text="Energy-Sufficient CPU Scheduling Simulator", 
                              font=self.title_font,
                              foreground="#2c3e50")
        title_label.pack(side=tk.LEFT)
        
      
        control_frame = ttk.LabelFrame(self.root, 
                                     text="Control Panel", 
                                     padding=(15, 10),
                                     style='TFrame')
        control_frame.pack(fill=tk.X, padx=10, pady=5)
        
      
        self.load_btn = ttk.Button(control_frame, 
                                  text="📂 Load Dataset", 
                                  command=self.load_dataset,
                                  style='TButton')
        self.load_btn.grid(row=0, column=0, padx=5, pady=5, sticky=tk.W)
        
        
        ttk.Label(control_frame, text="Scheduling Algorithm:").grid(row=0, column=1, padx=5, sticky=tk.E)
        
        self.algorithm_var = tk.StringVar()
        algorithms = [
            "First Come First Serve (FCFS)",
            "Shortest Job First (SJF)",
            "Shortest Remaining Time First (SRTF)",
            "Round Robin (RR)",
            "Priority with Preemption",
            "Priority without Preemption"
        ]
        
        self.algorithm_menu = ttk.Combobox(control_frame, 
                                         textvariable=self.algorithm_var, 
                                         values=algorithms, 
                                         state="readonly",
                                         width=30)
        self.algorithm_menu.grid(row=0, column=2, padx=5, sticky=tk.W)
        self.algorithm_menu.current(0)
        
        
        ttk.Label(control_frame, text="Time Quantum (for RR):").grid(row=0, column=3, padx=5, sticky=tk.E)
        self.quantum_entry = ttk.Entry(control_frame, width=5)
        self.quantum_entry.insert(0, "4")
        self.quantum_entry.grid(row=0, column=4, padx=5, sticky=tk.W)
        
      
        self.run_btn = ttk.Button(control_frame, 
                                 text="▶ Run Simulation", 
                                 command=self.run_algorithm,
                                 style='Accent.TButton')
        self.run_btn.grid(row=0, column=5, padx=10, pady=5)
        
        
        for i in range(6):
            control_frame.grid_columnconfigure(i, weight=1)
        
      
        content_frame = ttk.Frame(self.root, style='TFrame')
        content_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)
        
        
        left_panel = ttk.Frame(content_frame, style='TFrame')
        left_panel.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)
        
      
        dataset_frame = ttk.LabelFrame(left_panel, 
                                     text="Dataset Preview", 
                                     padding=(10, 5),
                                     style='TFrame')
        dataset_frame.pack(fill=tk.BOTH, expand=True, pady=(0, 10))
        
        self.dataset_text = tk.Text(dataset_frame, 
                                  height=10, 
                                  font=('Consolas', 10),
                                  wrap=tk.NONE)
        self.dataset_text.pack(fill=tk.BOTH, expand=True)
        
      
        y_scroll = ttk.Scrollbar(dataset_frame, orient=tk.VERTICAL, command=self.dataset_text.yview)
        y_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        x_scroll = ttk.Scrollbar(dataset_frame, orient=tk.HORIZONTAL, command=self.dataset_text.xview)
        x_scroll.pack(side=tk.BOTTOM, fill=tk.X)
        self.dataset_text.configure(yscrollcommand=y_scroll.set, xscrollcommand=x_scroll.set)
        
    
        self.gantt_frame = ttk.LabelFrame(left_panel, 
                                        text="Gantt Chart", 
                                        padding=(10, 5),
                                        style='TFrame')
        self.gantt_frame.pack(fill=tk.BOTH, expand=True)
        
        
        right_panel = ttk.Frame(content_frame, style='TFrame')
        right_panel.pack(side=tk.RIGHT, fill=tk.BOTH, expand=False, padx=5, pady=5)
        
    
        result_frame = ttk.LabelFrame(right_panel, 
                                     text="Scheduling Results", 
                                     padding=(10, 5),
                                     style='TFrame')
        result_frame.pack(fill=tk.BOTH, expand=True, pady=(0, 10))
        
    
        self.result_tree = ttk.Treeview(result_frame)
        self.result_tree.pack(fill=tk.BOTH, expand=True)
        
        # Add scrollbar
        tree_scroll = ttk.Scrollbar(result_frame, orient=tk.VERTICAL, command=self.result_tree.yview)
        tree_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.result_tree.configure(yscrollcommand=tree_scroll.set)
        
    
        stats_frame = ttk.LabelFrame(right_panel, 
                                   text="Performance Metrics", 
                                   padding=(10, 5),
                                   style='TFrame')
        stats_frame.pack(fill=tk.BOTH, expand=False)
        
    
        self.stats_text = tk.Text(stats_frame, 
                                height=4, 
                                font=('Helvetica', 10),
                                bg="#f8f9fa",
                                relief=tk.FLAT)
        self.stats_text.pack(fill=tk.BOTH, expand=True)
        
    
        self.status_var = tk.StringVar()
        self.status_var.set("Ready")
        status_bar = ttk.Label(self.root, 
                             textvariable=self.status_var,
                             relief=tk.SUNKEN,
                             anchor=tk.W,
                             style='TLabel')
        status_bar.pack(side=tk.BOTTOM, fill=tk.X)
        
        
        self.style.configure('Accent.TButton', 
                           foreground='white', 
                           background='#3498db',
                           font=self.button_font)
        
    def load_dataset(self):
        file_path = filedialog.askopenfilename(
            title="Select Dataset File",
            filetypes=(("CSV files", "*.csv"), ("All files", "*.*"))
        )
        
        if not file_path:
            return
        
        try:
            self.dataset = pd.read_csv(file_path)
            self.dataset_text.delete(1.0, tk.END)
            self.dataset_text.insert(tk.END, self.dataset.to_string(index=False))
            self.status_var.set(f"Dataset loaded successfully: {file_path}")
            messagebox.showinfo("Success", "Data loaded successfully")
            
          
            if 'Priority' not in self.dataset.columns:
                self.algorithm_menu['values'] = [
                    "First Come First Serve (FCFS)",
                    "Shortest Job First (SJF)",
                    "Shortest Remaining Time First (SRTF)",
                    "Round Robin (RR)"
                ]
            else:
                self.algorithm_menu['values'] = [
                    "First Come First Serve (FCFS)",
                    "Shortest Job First (SJF)",
                    "Shortest Remaining Time First (SRTF)",
                    "Round Robin (RR)",
                    "Priority with Preemption",
                    "Priority without Preemption"
                ]
            
        except Exception as e:
            messagebox.showerror("Error", f"Failed to load dataset: {str(e)}")
            self.status_var.set("Error loading dataset")
    
    def run_algorithm(self):
        if self.dataset is None:
            messagebox.showerror("Error", "Please load a dataset first")
            self.status_var.set("Error: No dataset loaded")
            return
        
        algorithm = self.algorithm_var.get()
        self.status_var.set(f"Running {algorithm}...")
        self.root.update()
        
        try:
            if algorithm == "First Come First Serve (FCFS)":
                results = self.fcfs()
            elif algorithm == "Shortest Job First (SJF)":
                results = self.sjf()
            elif algorithm == "Shortest Remaining Time First (SRTF)":
                results = self.srtf()
            elif algorithm == "Round Robin (RR)":
                quantum = int(self.quantum_entry.get())
                results = self.round_robin(quantum)
            elif algorithm == "Priority with Preemption":
                results = self.priority_preemptive()
            elif algorithm == "Priority without Preemption":
                results = self.priority_non_preemptive()
            else:
                messagebox.showerror("Error", "Invalid algorithm selected")
                self.status_var.set("Error: Invalid algorithm")
                return
            
            self.display_results(results)
            self.plot_gantt_chart(results)
            self.status_var.set(f"Completed: {algorithm} - Ready")
            
        except Exception as e:
            messagebox.showerror("Error", f"Failed to run algorithm: {str(e)}")
            self.status_var.set(f"Error running {algorithm}")
    
    def display_results(self, results):
        
        for item in self.result_tree.get_children():
            self.result_tree.delete(item)
        
    
        columns = list(results[0].keys())
        self.result_tree["columns"] = columns
        self.result_tree.column("#0", width=0, stretch=tk.NO)
        
        for col in columns:
            self.result_tree.column(col, anchor=tk.CENTER, width=100)
            self.result_tree.heading(col, text=col)
        
        
        for process in results:
            self.result_tree.insert("", tk.END, values=tuple(process.values()))
        
    
        total_tat = sum(process['Turnaround Time'] for process in results)
        avg_tat = total_tat / len(results)
        total_wt = sum(process['Waiting Time'] for process in results)
        avg_wt = total_wt / len(results)
        
        
        self.stats_text.delete(1.0, tk.END)
        stats = f"╔{'═'*65}╗\n"
        stats += f"║ {'Total Turnaround Time:':<30}{total_tat:>10.2f}{' '*23}║\n"
        stats += f"║ {'Average Turnaround Time:':<30}{avg_tat:>10.2f}{' '*23}║\n"
        stats += f"║ {'Total Waiting Time:':<30}{total_wt:>10.2f}{' '*23}║\n"
        stats += f"║ {'Average Waiting Time:':<30}{avg_wt:>10.2f}{' '*23}║\n"
        stats += f"╚{'═'*65}╝"
        self.stats_text.insert(tk.END, stats)
        self.stats_text.tag_configure("center", justify='center')
        self.stats_text.tag_add("center", "1.0", "end")
    
    def plot_gantt_chart(self, results):
        # Clear previous gantt chart
        for widget in self.gantt_frame.winfo_children():
            widget.destroy()
        
      
        processes = []
        start_times = []
        durations = []
        
        for process in results:
            processes.append(process['Process'])
            start_times.append(process['Start Time'])
            durations.append(process['Completion Time'] - process['Start Time'])
        
    
        fig, ax = plt.subplots(figsize=(10, 3))
        colors = plt.cm.tab20.colors
        
  
        y_pos = range(len(processes))
        ax.barh(y_pos, durations, left=start_times, color=colors, edgecolor='black')
        
        
        ax.set_yticks(y_pos)
        ax.set_yticklabels(processes)
        ax.set_xlabel('Time')
        ax.set_title('Gantt Chart')
        ax.grid(True, which='both', axis='x', linestyle='--', alpha=0.7)
        
      
        for i, (start, duration) in enumerate(zip(start_times, durations)):
            ax.text(start + duration/2, i, f"{duration}", 
                   ha='center', va='center', color='black')
        
      
        canvas = FigureCanvasTkAgg(fig, master=self.gantt_frame)
        canvas.draw()
        canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
    
    
    def fcfs(self):
        processes = self.dataset.copy()
        processes = processes.sort_values(by='Arrival Time')
        
        results = []
        current_time = 0
        
        for _, process in processes.iterrows():
            arrival = process['Arrival Time']
            burst = process['Burst Time']
            
            if current_time < arrival:
                current_time = arrival
            
            start_time = current_time
            completion_time = start_time + burst
            turnaround_time = completion_time - arrival
            waiting_time = start_time - arrival
            
            results.append({
                'Process': process['Process'],
                'Arrival Time': arrival,
                'Burst Time': burst,
                'Start Time': start_time,
                'Completion Time': completion_time,
                'Turnaround Time': turnaround_time,
                'Waiting Time': waiting_time
            })
            
            current_time = completion_time
        
        return results
    
    def sjf(self):
        processes = self.dataset.copy()
        processes = processes.sort_values(by='Arrival Time')
        
        results = []
        current_time = 0
        n = len(processes)
        completed = 0
        remaining = processes.copy()
        
        while completed != n:
            # Get processes that have arrived
            ready = remaining[remaining['Arrival Time'] <= current_time]
            
            if ready.empty:
                current_time += 1
                continue
            
          
            shortest = ready.loc[ready['Burst Time'].idxmin()]
            
            start_time = current_time
            completion_time = start_time + shortest['Burst Time']
            turnaround_time = completion_time - shortest['Arrival Time']
            waiting_time = start_time - shortest['Arrival Time']
            
            results.append({
                'Process': shortest['Process'],
                'Arrival Time': shortest['Arrival Time'],
                'Burst Time': shortest['Burst Time'],
                'Start Time': start_time,
                'Completion Time': completion_time,
                'Turnaround Time': turnaround_time,
                'Waiting Time': waiting_time
            })
            
            current_time = completion_time
            remaining = remaining.drop(shortest.name)
            completed += 1
        
        return results
    
    def srtf(self):
        processes = self.dataset.copy()
        processes = processes.sort_values(by='Arrival Time')
        
        results = []
        n = len(processes)
        remaining_time = processes['Burst Time'].values.copy()
        completed = 0
        current_time = 0
        
        
        process_info = {row['Process']: {
            'arrival': row['Arrival Time'],
            'burst': row['Burst Time'],
            'start': -1,
            'completion': -1,
            'remaining': row['Burst Time']
        } for _, row in processes.iterrows()}
        
        while completed != n:
          
            ready = [p for p in process_info 
                    if process_info[p]['arrival'] <= current_time and 
                    process_info[p]['remaining'] > 0]
            
            if not ready:
                current_time += 1
                continue
            
      
            next_process = min(ready, key=lambda x: process_info[x]['remaining'])
            
          
            if process_info[next_process]['start'] == -1:
                process_info[next_process]['start'] = current_time
            
        
            process_info[next_process]['remaining'] -= 1
            current_time += 1
            
            
            if process_info[next_process]['remaining'] == 0:
                completed += 1
                process_info[next_process]['completion'] = current_time
        
        
        for process in process_info:
            info = process_info[process]
            results.append({
                'Process': process,
                'Arrival Time': info['arrival'],
                'Burst Time': info['burst'],
                'Start Time': info['start'],
                'Completion Time': info['completion'],
                'Turnaround Time': info['completion'] - info['arrival'],
                'Waiting Time': (info['completion'] - info['arrival']) - info['burst']
            })
        
        
        results.sort(key=lambda x: x['Completion Time'])
        return results
    
    def round_robin(self, quantum):
        processes = self.dataset.copy()
        processes = processes.sort_values(by='Arrival Time')
        
        results = []
        n = len(processes)
        remaining_time = processes['Burst Time'].values.copy()
        completed = 0
        current_time = 0
        
        
        process_info = {row['Process']: {
            'arrival': row['Arrival Time'],
            'burst': row['Burst Time'],
            'start': -1,
            'completion': -1,
            'remaining': row['Burst Time']
        } for _, row in processes.iterrows()}
        
        queue = []
        arrived = set()
        
        
        for _, process in processes.iterrows():
            if process['Arrival Time'] == 0:
                queue.append(process['Process'])
                arrived.add(process['Process'])
        
        while completed != n:
            if not queue:
                current_time += 1
                # Check for new arrivals
                for _, process in processes.iterrows():
                    if (process['Arrival Time'] <= current_time and 
                        process['Process'] not in arrived and 
                        process_info[process['Process']]['remaining'] > 0):
                        queue.append(process['Process'])
                        arrived.add(process['Process'])
                continue
            
            current_process = queue.pop(0)
            
          
            if process_info[current_process]['start'] == -1:
                process_info[current_process]['start'] = current_time
            
            
            exec_time = min(quantum, process_info[current_process]['remaining'])
            process_info[current_process]['remaining'] -= exec_time
            current_time += exec_time
            
        
            for _, process in processes.iterrows():
                if (process['Arrival Time'] <= current_time and 
                    process['Process'] not in arrived and 
                    process_info[process['Process']]['remaining'] > 0):
                    queue.append(process['Process'])
                    arrived.add(process['Process'])
            
        
            if process_info[current_process]['remaining'] == 0:
                completed += 1
                process_info[current_process]['completion'] = current_time
            else:
                # Put back in queue
                queue.append(current_process)
        
      
        for process in process_info:
            info = process_info[process]
            results.append({
                'Process': process,
                'Arrival Time': info['arrival'],
                'Burst Time': info['burst'],
                'Start Time': info['start'],
                'Completion Time': info['completion'],
                'Turnaround Time': info['completion'] - info['arrival'],
                'Waiting Time': (info['completion'] - info['arrival']) - info['burst']
            })
        
    
        results.sort(key=lambda x: x['Completion Time'])
        return results
    
    def priority_preemptive(self):
        processes = self.dataset.copy()
        processes = processes.sort_values(by='Arrival Time')
        
        results = []
        n = len(processes)
        remaining_time = processes['Burst Time'].values.copy()
        completed = 0
        current_time = 0
        
        
        process_info = {row['Process']: {
            'arrival': row['Arrival Time'],
            'burst': row['Burst Time'],
            'priority': row['Priority'],
            'start': -1,
            'completion': -1,
            'remaining': row['Burst Time']
        } for _, row in processes.iterrows()}
        
        while completed != n:
        
            ready = [p for p in process_info 
                    if process_info[p]['arrival'] <= current_time and 
                    process_info[p]['remaining'] > 0]
            
            if not ready:
                current_time += 1
                continue
            
      
            next_process = min(ready, key=lambda x: process_info[x]['priority'])
            
           
            if process_info[next_process]['start'] == -1:
                process_info[next_process]['start'] = current_time
            
          
            process_info[next_process]['remaining'] -= 1
            current_time += 1
            
           
            if process_info[next_process]['remaining'] == 0:
                completed += 1
                process_info[next_process]['completion'] = current_time
        
      
        for process in process_info:
            info = process_info[process]
            results.append({
                'Process': process,
                'Arrival Time': info['arrival'],
                'Burst Time': info['burst'],
                'Priority': info['priority'],
                'Start Time': info['start'],
                'Completion Time': info['completion'],
                'Turnaround Time': info['completion'] - info['arrival'],
                'Waiting Time': (info['completion'] - info['arrival']) - info['burst']
            })
        

        results.sort(key=lambda x: x['Completion Time'])
        return results
    
    def priority_non_preemptive(self):
        processes = self.dataset.copy()
        processes = processes.sort_values(by='Arrival Time')
        
        results = []
        current_time = 0
        n = len(processes)
        completed = 0
        remaining = processes.copy()
        
        while completed != n:
          
            ready = remaining[remaining['Arrival Time'] <= current_time]
            
            if ready.empty:
                current_time += 1
                continue
            
           
            highest_priority = ready.loc[ready['Priority'].idxmin()]
            
            start_time = current_time
            completion_time = start_time + highest_priority['Burst Time']
            turnaround_time = completion_time - highest_priority['Arrival Time']
            waiting_time = start_time - highest_priority['Arrival Time']
            
            results.append({
                'Process': highest_priority['Process'],
                'Arrival Time': highest_priority['Arrival Time'],
                'Burst Time': highest_priority['Burst Time'],
                'Priority': highest_priority['Priority'],
                'Start Time': start_time,
                'Completion Time': completion_time,
                'Turnaround Time': turnaround_time,
                'Waiting Time': waiting_time
            })
            
            current_time = completion_time
            remaining = remaining.drop(highest_priority.name) 
            completed += 1
        
        return results

if __name__ == "__main__":
    root = tk.Tk()
    app = CPUSchedulingApp(root)
    root.mainloop()
