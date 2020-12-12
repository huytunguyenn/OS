## ***Tìm hiểu & Lập trình Linux kernel module***

Đồ án 2 môn Hệ điều hành - lớp CQ2018/21.

> Viết một module dùng để tạo ra số ngẫu nhiên (RNG - Random Number Generator).

> Module này sẽ tạo một character device để cho phép các tiến trình ở user space có thể open và read các số ngẫu nhiên.

## Thông tin sinh viên

 

+ Nguyễn Huy Tú - 18120254

+ Nguyễn Duy Vũ - 18120264

## Thông tin bài làm


Bao gồm: thư mục *Source* chứa mã nguồn và thư mục *Document* chứa báo cáo đồ án.

Điểm đề nghị cho bài tập: 10


- Các chức năng đã làm được: 100%


	
- Các chức năng chưa làm được: 0%



## Hướng dẫn sử dụng

Đi tới thư mục chứa mã nguồn:

	cd RNG_LKM

Build kernel module theo phương pháp Kbuild bằng lệnh:

	make

Cài đặt module vào kernel.

	sudo insmod randomModule.ko

Gọi hàm ở user space để đọc dữ liệu số ngẫu nhiên được tạo từ character device ở kernel space.

	sudo ./userTest

Tháo module ra khỏi kernel.

	sudo rmmod randomModule

Dọn dẹp file build dư thừa trong folder.

	make clean
	



