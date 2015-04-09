#	 Copyright 2015 SimpleThings, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


CFLAGS_INCLUDES		+= -I. -I../../include -I../

CFLAGS				+= $(CFLAGS_INCLUDES) -g -c  -fPIC 

OBJ_FILES			=		linux_os.o	\
							linux_communication.o

SO_TARGET   		:=      libcanopy_os.so 
A_TARGET   			+=		libcanopy_os.a

NEEDED_H_FILES		=		../../include/canopy_os.h	\
							../canopy_communication.h


default: $(SO_TARGET) $(A_TARGET)

clean:
	rm -rf *.o *.d *.so *.a

install:
	@echo "Copying libraries"
	cp libcanopy_os.so /usr/lib/

%.o: %.c
	@echo " [cc]  $<"
	$(CC) -c -o $@ $(CFLAGS) $(EXTRACFLAGS) $<


libcanopy_os.so: $(OBJ_FILES) $(NEEDED_H_FILES)
	$(CC) -shared  -o libcanopy_os.so $(OBJ_FILES)

                   
libcanopy_os.a:  $(OBJ_FILES) $(NEEDED_H_FILES)
	$(AR) rc $@ $(OBJ_FILES)
 
 
 
 
