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


CFLAGS_INCLUDES	 += -I. -I../../include

CFLAGS += $(CFLAGS_INCLUDES) -g -c

SOURCE_FILES	=	linux_os.c
OBJECT_FILES    =   linux_os.o

SO_TARGET   :=      libcanopy_os.so 
A_TARGET    +=      libcanopy_os.a

NEEDED_H_FILES	=	../../include/canopy_os.h

default: $(SO_TARGET)

clean:
	rm -rf *.o *.d *.so *.a

install:
	@echo "No install target"

libcanopy_os.so: $(SOURCE_FILES) $(NEEDED_H_FILES)
	$(CC) -fPIC -rdynamic -shared \
                   $(CFLAGS_INCLUDES) $(SOURCE_FILES) $(CFLAGS) \
                   -o libcanopy_os.so

                   
 #libcanopy_os.a:  $(SOURCE_FILES) $(OBJECT_FILES) $(NEEDED_H_FILES)
 #   $(AR) rc $@ $(OBJECT_FILES)
 
 
 