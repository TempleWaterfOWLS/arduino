''' 
Code written to ping the Arduino-Based webserver for information. 
Intended to recover hex-digit information from the server, which indicates
RPM data, and possibly other data ( I have no idea what the motor controllers return)
INPUTS: None
Outputs: Writes data to text file, 'motor_hex.txt' Written by Zack Smith 10/01/14
'''
# Imports
import requests
import sys
from time import strftime, sleep
# Function designed to take input hex data and write to motor_hex.txt, along with timestamp formatted Year-month-day hour:minute:second
def append_to_file(hex_data):
        with open(sys.argv[1], 'a') as data_file:
                data_file.write("Data Request at time: ")
                data_file.write(strftime("%Y-%m-%d %H:%M:%S \n"))
                data_file.write(str(hex_data) + '\n')
        return
        
# Function designed to parse response for motor information
def parse_response(get_response):
	return get_response.text

# Function designed to print information about the get response for debugging purposes
def print_response(get_response):
	print "URL Accesed"
	print get_response.url
	print "Status Code:"
	print get_response.status_code
	print "Headers:"
	print get_response.headers
	print "Text content:"
	print get_response.text
        return

def main():
	# Declare serverIP, port, and desired index to create URL
	serverIP = "192.168.0.103"; port="80"; directory = "/pythoninfo";
	url="http://"+serverIP+":"+port+directory
        # Continually send requests
        while True:
		print "Getting Response..."
                get_response = requests.get(url)
                # Print out useful debugging information about the request
                print_response(get_response)
                # Parse response for hex data
		print "Parsing Response..."
                hex_data = parse_response(get_response)

		idx_rpm = [11, 12, 13, 14]
		idx_volts = [15, 16, 17, 18]
		idx_amps = [19, 20, 21, 22]
		idx_temp = [23, 24, 25, 26]
		rpm_hex = [hex_data[i] for i in idx_rpm]
		volts_hex = [hex_data[i] for i in idx_volts]
		amps_hex = [hex_data[i] for i in idx_amps]
		temp_hex = [hex_data[i] for i in idx_idx]
		for x in range (0, len(rpm_hex));
		rpm_hex_array += rpm_hex[x]
		for x in range (0, len(volts_hex));
		volts_hex_array += volts_hex[x]
		for x in range (0, len(amps_hex));
		amps_hex_array += amps_hax[x]
		for x in range (0, len(temp_hex));
		temp_hex_array += temp_hex[x]
		rpm = struct.unpack('!f', 'rpm_hex_array'.decode('hex'))[0]
		volts = struct.unpack('!f', 'volts_hex_array'.decode('hex'))[0]
		amps = struct.unpack('!f', 'amps_hex_array'.decode('hex'))[0]
		temp = struct.unpack('!f', 'temp_hex_array'.decode('hex'))[0]
		print "The rpms of the motors are: %/f" %rpm
		print "The voltage on the bus is: %f" %volts
		print "The amperage on the bus is: %f" %amps
		print "The Tempature of the control boards is:%f" %temp
		float_values_split = hex_data.split('\n');
                float_values = struct.unpack('!f', 'float_values_split'.decode('hex'))[0]
		print float_values
		# Write hex data to file with time stamp
                print "Appending to file..."
		append_to_file(hex_data)
                # Sleep to not overload server with requests
              	print "Sleeping for 300ms"
		sleep(0.3)

# Call main boiler plate
if __name__ == '__main__':
        main()
