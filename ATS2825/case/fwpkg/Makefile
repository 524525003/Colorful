# ��makefile�����Զ���ɹ������ã�������룬�̼����ɵȹ��ܣ������ʵ������޸ĸ�makefile���Է���
# ��������Ĺ̼�
# makefile���������shell�ļ�
#       config.sh���ڹ������ã��ɸ������õİ��ͺ͹����Ƿ�֧�֣����޸Ĳ����ļ�����ɹ�����Ĵ򿪺͹ر�
#       make.sh ����Դ������룬��shell�ű������case/cfgĿ¼�µ�makefile���г������
#       build.sh ���ڹ̼����ɣ��ɸ������õİ��ͺ͹����Ƿ�֧�֣�������Ӧ��Ŀ��̼����ýű�������fw�ļ�������������������
#                hex��fwu�ļ������������ܣ�atf�ļ�����ATT�����Զ������ԣ��Լ�OTA�ļ�����apk����
# ������ù����У���һ��shell�ű�������������������̻�ֹͣ������������shell�ű������������Ӧ��log�ļ�����ͨ������log�ļ���������ԭ��

# ���������һ��֧��TWS���ܵ�ATS2823�Ĺ̼�����������������make clean;make DVB_ATS2823_TWS,���Կ���DVB_ATS2823_TWS��һ��αĿ�꣬��Ŀ������
# config.sh,��TWS���ܴ򿪡����ŵ���make.sh��build.sh��ɹ̼�������

# ���֮ǰԴ�����Ѿ��������ֻ���޸���ĳ���ļ������ٴ����ɹ̼���������make DVB_ATS2823_TWS,��ɳ���ı���

# ���Դ�ļ���û���޸ģ�����ֻ���ٴ����ɹ̼���������make build_DVB_ATS2823_TWS��make build��ɹ̼�������

.PHONY : all clean clean_target

all:
	#make clean
	#make EVB
	make clean
	make DVB_ATS2825
	make clean
	make DVB_ATS2823
	#make clean
	#make EVB_OTA
	#make clean
	#make DVB_ATS2823_OTA
	#make clean
	#make EVB_TWS
	make clean
	make DVB_ATS2825_TWS
	make clean
	make DVB_ATS2823_TWS
	make clean
	make DVB_ATS2825_OTA	
EVB:
	./config.sh US282A_BTBOX_EVB 0 0 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh US282A_BTBOX_EVB US282A_BTBOX_EVB 2>&1 | tee ./build.log
		
DVB_ATS2825:
	./config.sh US282A_BTBOX_DVB_ATS2825 0 0 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh US282A_BTBOX_DVB_ATS2825 US282A_BTBOX_DVB_ATS2825 2>&1 | tee ./build.log	
	
DVB_ATS2823:
	./config.sh US282A_BTBOX_DVB_ATS2823 0 0 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh US282A_BTBOX_DVB_ATS2823 US282A_BTBOX_DVB_ATS2823 2>&1 | tee ./build.log		
	
EVB_OTA:
	./config.sh US282A_BTBOX_EVB 1 0 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh US282A_BTBOX_EVB US282A_BTBOX_EVB_OTA 2>&1 | tee ./build.log	
	
DVB_ATS2825_OTA:
	./config.sh US282A_BTBOX_DVB_ATS2825 1 0 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh US282A_BTBOX_DVB_ATS2825 US282A_BTBOX_DVB_ATS2825_OTA 2>&1 | tee ./build.log	

#DVB_ATS2823_OTA:
#	./config.sh US282A_BTBOX_DVB_ATS2823 1 0 2>&1 | tee ./config.log
#	./make.sh 2 2>&1 | tee ./make.log
#	./build.sh US282A_BTBOX_DVB_ATS2823 US282A_BTBOX_DVB_ATS2823_OTA 2>&1 | tee ./build.log

EVB_TWS:
	./config.sh US282A_BTBOX_EVB 0 1 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh US282A_BTBOX_EVB US282A_BTBOX_EVB_TWS 2>&1 | tee ./build.log

DVB_ATS2825_TWS:
	./config.sh US282A_BTBOX_DVB_ATS2825 0 1 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh US282A_BTBOX_DVB_ATS2825 US282A_BTBOX_DVB_ATS2825_TWS 2>&1 | tee ./build.log	
	
#DVB_ATS2825_1016_TWS:
#	cp -f ./config_txt/US282A_BTBOX_DVB_ATS2825_1016.txt ./config_txt/US282A_BTBOX_DVB_ATS2825.txt
#	./config.sh US282A_BTBOX_DVB_ATS2825 0 1 2>&1 | tee ./config.log
#	./make.sh 2 2>&1 | tee ./make.log
#	./build.sh US282A_BTBOX_DVB_ATS2825 US282A_BTBOX_DVB_ATS2825_1016_TWS 2>&1 | tee ./build.log		
	
#DVB_ATS2825_2119_TWS:
#	cp -f ./config_txt/US282A_BTBOX_DVB_ATS2825_2119.txt ./config_txt/US282A_BTBOX_DVB_ATS2825.txt
#
#	./make.sh 2 2>&1 | tee ./make.log
#	./build.sh US282A_BTBOX_DVB_ATS2825 US282A_BTBOX_DVB_ATS2825_2119_TWS 2>&1 | tee ./build.log		
	
DVB_ATS2823_TWS:
	./config.sh US282A_BTBOX_DVB_ATS2823 0 1 2>&1 | tee ./config.log
	./make.sh 2 2>&1 | tee ./make.log
	./build.sh US282A_BTBOX_DVB_ATS2823 US282A_BTBOX_DVB_ATS2823_TWS 2>&1 | tee ./build.log
	
build_EVB:
	./build.sh US282A_BTBOX_EVB US282A_BTBOX_EVB 2>&1 | tee ./build.log    	

build_DVB_ATS2825:
	./build.sh US282A_BTBOX_DVB_ATS2825 US282A_BTBOX_DVB_ATS2825 2>&1 | tee ./build.log	

build_DVB_ATS2823:
	./build.sh US282A_BTBOX_DVB_ATS2823 US282A_BTBOX_DVB_ATS2823 2>&1 | tee ./build.log

build_EVB_OTA:
	./build.sh US282A_BTBOX_EVB US282A_BTBOX_EVB_OTA 2>&1 | tee ./build.log	

build_DVB_ATS2825_OTA:
	./build.sh US282A_BTBOX_DVB_ATS2825 US282A_BTBOX_DVB_ATS2825_OTA 2>&1 | tee ./build.log

build_EVB_TWS:
	./build.sh US282A_BTBOX_EVB US282A_BTBOX_EVB_TWS 2>&1 | tee ./build.log

build_DVB_ATS2825_TWS:
	./build.sh US282A_BTBOX_DVB_ATS2825 US282A_BTBOX_DVB_ATS2825_TWS 2>&1 | tee ./build.log

build_DVB_ATS2823_TWS:
	./build.sh US282A_BTBOX_DVB_ATS2823 US282A_BTBOX_DVB_ATS2823_TWS 2>&1 | tee ./build.log	

build:
	./build.sh  2>&1 | tee ./build.log  
	  
clean:
	./make.sh 0 2>&1 | tee ./make.log

clean_target:
	./make.sh 1 2>&1 | tee ./make.log
			
	