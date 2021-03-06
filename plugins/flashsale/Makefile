# Please tweak it to suit your environment and project.  You
# may want to move it to your project's root directory.
#
# SYNOPSIS:
#
#   make [all]  - makes everything.
#   make TARGET - makes the given target.
#   make clean  - removes all files generated by make.


# Where to find user code.

LIB_DIR = ./../../../library
USER_DIR = ./../../pub
SO_DIR = ./../../bin/plugins

THIRD_PATH = $(LIB_DIR)/mylib/public

CUSTOM_THIRD_PATH = $(LIB_DIR)/mylib/third_party

LINK_PATHS = -L./ -L/usr/local/libmemcached-1.0.7/lib/  -L/usr/lib64/mysql

PLUGIN = $(SO_DIR)/flash/flash.so
PLUGIN_TYPE = "LIB"
                
INCLUDE_PATHS = -I/usr/local/libmemcached-1.0.7/include \
				-I$(LIB_DIR)/BasicCore \
				-I$(THIRD_PATH)\
				-I/usr/local/include\
				-I/usr/include/mysql\
				-I$(USER_DIR)\
				-I$(CUSTOM_THIRD_PATH)/hiredis\
				-I$(CUSTOM_THIRD_PATH)\
				-I$(CUSTOM_THIRD_PATH)/librdkafka/src\
				-I$(LIB_DIR)/mylib/base\
				-I../\
				


# House-keeping build targets.

ALL_HEADERS = $(USER_HEADERS)
all : $(PLUGIN)

clean :
	rm -f $(PLUGIN)  *.o

# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
#GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
#gtest-all.o : $(GTEST_SRCS_)
#	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

#gtest_main.o : $(GTEST_SRCS_)
#	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

#gtest.a : gtest-all.o
#	$(AR) $(ARFLAGS) $@ $^

#gtest_main.a : gtest-all.o gtest_main.o
#	$(AR) $(ARFLAGS) $@ $^

# Builds a sample test.  A test should link with either gtest.a or
# gtest_main.a, depending on whether it defines its own main()
# function.


flash_init.o : ./flash_init.cc ./flash_init.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c ./flash_init.cc $(INCLUDE_PATHS)

flash_logic.o : ./flash_logic.cc ./flash_logic.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c ./flash_logic.cc $(INCLUDE_PATHS)

flash_db.o : ./flash_db.cc ./flash_db.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c ./flash_db.cc $(INCLUDE_PATHS)

share_memory.o : ./share_memory.cc ./share_memory.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c ./share_memory.cc $(INCLUDE_PATHS)

flash_info.o : ./flash_info.cc ./flash_info.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c ./flash_info.cc $(INCLUDE_PATHS)

flash_kafka.o : ./flash_kafka.cc ./flash_kafka.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c ./flash_kafka.cc $(INCLUDE_PATHS)

schduler_engine.o : ./schduler_engine.cc ./schduler_engine.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c ./schduler_engine.cc $(INCLUDE_PATHS)

trades_proto_buf.o : ./trades_proto_buf.cc ./trades_proto_buf.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c ./trades_proto_buf.cc $(INCLUDE_PATHS)

logic_unit.o :  $(USER_DIR)/logic/logic_unit.cc $(USER_DIR)/logic/logic_unit.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/logic/logic_unit.cc $(INCLUDE_PATHS)

star_infos.o :  $(USER_DIR)/logic/star_infos.cc $(USER_DIR)/logic/star_infos.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/logic/star_infos.cc $(INCLUDE_PATHS)

packet_processing.o :  $(USER_DIR)/net/packet_processing.cc $(USER_DIR)/net/packet_processing.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/net/packet_processing.cc $(INCLUDE_PATHS)

data_engine.o :  $(USER_DIR)/storage/data_engine.cc $(USER_DIR)/storage/data_engine.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/storage/data_engine.cc $(INCLUDE_PATHS)
	
data_mysql_engine.o :  $(USER_DIR)/storage/data_mysql_engine.cc $(USER_DIR)/storage/data_mysql_engine.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/storage/data_mysql_engine.cc $(INCLUDE_PATHS)
	
data_redis_engine.o :  $(USER_DIR)/storage/data_redis_engine.cc $(USER_DIR)/storage/data_redis_engine.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/storage/data_redis_engine.cc $(INCLUDE_PATHS)
	
mysql_pool.o :  $(USER_DIR)/storage/mysql_pool.cc $(USER_DIR)/storage/mysql_pool.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/storage/mysql_pool.cc $(INCLUDE_PATHS)

redis_pool.o :  $(USER_DIR)/storage/redis_pool.cc $(USER_DIR)/storage/redis_pool.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/storage/redis_pool.cc $(INCLUDE_PATHS)

time.o :  $(USER_DIR)/logic/time.cc $(USER_DIR)/logic/time.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/logic/time.cc $(INCLUDE_PATHS)

$(PLUGIN) : flash_init.o  flash_logic.o flash_db.o share_memory.o flash_info.o flash_kafka.o schduler_engine.o \
	star_infos.o time.o logic_unit.o packet_processing.o data_engine.o data_mysql_engine.o data_redis_engine.o mysql_pool.o redis_pool.o\
	$(CUSTOM_THIRD_PATH)/dmg_fp/lib/dmg_fp.a $(LIB_DIR)/mylib/base/lib/mybase.a $(LIB_DIR)/mylib/third_party/lib/libhiredis.a $(CUSTOM_THIRD_PATH)/librdkafka/librdkafka.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LINK_PATHS) -lpthread -lexpat -lrt -ldl -lmysqlclient \
	$^ -o $@

include ../base.mk
