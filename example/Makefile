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

CFLAGS += $(CFLAGS_INCLUDES) -g

PROGRAM_FILES	=	toaster

LIBS	=	-L../src/ -lcanopy -L../src/linux -lcanopy_os -L../src/jsmn -ljsmn -lcurl

NEEDED_H_FILES	= \
        ../include/canopy_min.h \
        ../include/canopy_os.h

default: all

all: $(PROGRAM_FILES)

toaster: toaster.c $(NEEDED_H_FILES)
	$(CC) $(CFLAGS) toaster.c -g -o toaster $(LIBS)

clean:
	rm -rf $(PROGRAM_FILES) *.o *.d

install:
	@echo "No install target"
	



