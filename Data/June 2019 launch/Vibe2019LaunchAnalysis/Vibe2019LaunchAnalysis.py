import pandas as pd
import numpy as np
import matplotlib
#matplotlib.use('Agg')
import matplotlib.pyplot as plt

names=('X-Axis','Y-Axis', 'Z-Axis')

dirName=r"C:\Users\walte.DESKTOP-KBFHJTV\OneDrive\Documents\Space Team\RockSatX 2018-2019\June 2019 launch"
filename = dirName + r"\FlightResults.csv"

data = pd.read_csv(filename)

print(data.columns)


plt.figure(1)
plt.title("X Axis")
x = data["Index"]
y = data["X-Axis"]
plt.plot(x, y)

plt.figure(2)
plt.title("Y Axis")
y1 = data["Y-Axis"]
plt.plot(x, y1)

plt.figure(3)
plt.title("Z Axis")
y2 = data["Z-Axis"]
plt.plot(x, y2)

plt.show()

# importing libraries 
#import matplotlib.pyplot as plt 
#import pandas as pd 
#import numpy as np 
  
#ts = pd.Series(np.random.randn(1000), index = pd.date_range( 
#                                '1/1/2000', periods = 1000)) 
  
#df = pd.DataFrame(np.random.randn(1000, 4), index = ts.index, 
#                                      columns = list('ABCD')) 
  
#df3 = pd.DataFrame(np.random.randn(1000, 2), 
#               columns =['B', 'C']).cumsum() 
  
#df3['A'] = pd.Series(list(range(len(df)))) 
#df3.plot(x ='A', y ='B') 
#plt.show() 
