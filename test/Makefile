
objects = rtmp_client.o amf0.o types.o io.o 

rtmp_client:$(objects) 
			cc -g -o rtmp_client $(objects)

rtmp_client.o:amf0.h types.h io.h amf0.b

amf0.o:amf0.h amf0.b

types.o:types.h amf0.b

io.o:io.h

.PHONY : clean 
clean : 
	rm rtmp_client $(objects)
