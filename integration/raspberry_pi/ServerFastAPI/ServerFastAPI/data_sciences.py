import pandas as pd

# Define the file path
file_path = r'\\192.168.1.25\Raspberry\Bioreactor\ServerFastAPI\data\data.csv'

# Read the CSV file into a DataFrame
df = pd.read_csv(file_path)

# Display the DataFrame
#print(df.head())
print(df.info())
