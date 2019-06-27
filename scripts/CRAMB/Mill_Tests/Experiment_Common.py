import numpy as np
import pandas as pd

def extract_averages( path, skiprows = 38 ):
    
    #this function takes the averages over each column and writes it to a new text file
    
    #skiprows = 38... the header of each text file is 38 lines long
    arr = np.loadtxt( path, skiprows = skiprows) #load the data
    arr = arr[:,1::] #get rid of the first column which is time because it is not needed
    avg = np.average(arr, axis = 0)
    
    columns = 'My Fz Fx Fy Mx Mz'.split() #this is the order in of the columns in the data files
    data = pd.Series(data = avg, index = columns)
    
    #return data frame instead of series
    #avg = np.atleast_2d(avg)
    #data = pd.DataFrame(data = avg, columns = columns) #using the global variable 'columnns'
        
    return data

def command_que(cmds, execute_obj, info = None):
    
    #execute_obj needs to be callable and have a quit method
    
    N = len(cmds) #determine number of commands
    if info == None:
        info = ['' for i in range(N)]
    
    cmd_count = 0
    
    while cmd_count <= N:
        
        if cmd_count == 0:
            print("  START  ".center(50, '#'))
        elif cmd_count == N-1:
            print("  End  ".center(50, '#'))
        else:
            print("#"*50)
        
        #Extract current command string and info
        current_command = cmds[cmd_count]
        current_info = info[cmd_count]
        
        print(f"Info: {current_info}")
        print(f"Current Command: {current_command}")
        
        user_options = ['(H/h) -- Help Menu',
                        '(N/n) -- Next Command',
                        '(P/p) -- Previous Command',
                        '(E/e) -- Execute Command',
                        '(Q/q) -- Quit']
    
        help_menu = "\n" + " Help Menu ".center(20, '_')\
        + "\n" + "\n".join(user_options) + "\n"
        
        user = input("\nEnter a command: ").lower()
        
        if user == 'h':
            print(help_menu)        
        elif user == 'n':
            cmd_count += 1
        elif user == 'p':
            cmd_count -= 1
        elif user == 'e':
            try:
                execute_obj(current_command, cmd_count)
                print('Executed')
                cmd_count+=1 #move to next command after executing
            except:
                print('Not Executed!!')
        elif user == 'q':
            execute_obj.quit()
            print('  DONE  '.center(50, '#'))
            break
        else:
            print('Invalid Input')
        
        if cmd_count > N-1:
            cmd_count = N-1
        elif cmd_count < 0:
            cmd_count = 0
            
        #clear_output()
        
    print("\nDone with all commands!\n")
    return execute_obj