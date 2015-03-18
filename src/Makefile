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


CFLAGS_INCLUDES	 += -I. -I../include

CFLAGS 		= $(CFLAGS_INCLUDES) -g -fPIC -Wno-incompatible-pointer-types

OBJ_FILES := \
 		canopy_filters.o    \
		canopy_system.o		\
		canopy_remotes.o	\
		canopy_variables.o	\
		canopy_device.o		\
		canopy_json.o


SO_TARGET := libcanopy.so
A_TARGET := libcanopy.a

NEEDED_H_FILES	=	../include/canopy_min.h    \
                    ../include/canopy_min_internal.h \
                    ../include/canopy_os.h		\
                    ../include/canopy_http.h
                    

default: $(SO_TARGET) $(A_TARGET)

clean:
	rm -rf *.o *.d *.so *.a

install:
	@echo "Copying libraries"
	cp libcanopy.so /usr/lib/
	

VPATH = src
%.o: %.c
	@echo " [cc]  $<"
	$(CC) -c -o $@ $(CFLAGS) $(EXTRACFLAGS) $<


libcanopy.so: $(OBJ_FILES) $(NEEDED_H_FILES)
	$(CC) -shared  -o libcanopy.so $(OBJ_FILES)

                   
libcanopy.a:  $(OBJ_FILES) $(NEEDED_H_FILES)
	$(AR) rc $@ $(OBJ_FILES)
 
 

