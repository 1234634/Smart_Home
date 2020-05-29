all: device kontroler

kontroler: 
	g++ -o server kontroler.cpp -lpthread
	
device: 
	g++ -o device device.cpp -lpthread

clean:
	rm device server
