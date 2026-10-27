Scale dự án
1.Xây dựng hệ thống đăng nhập và phân quyền:
-Tại phần model ta thêm file user.h định nghĩa class user gồm tên đăng nhập,mật khẩu và role
-lưu trữ json cho các file user
-tại phần core ta thêm 1 class authmanager để quản lí trạng thái xem ai đăng nhập và phân quyền
-giao diện tương ứng bao gồm 1 thanh đăng nhập tài khoản và mật khẩu,sau khi đăng nhập xong thì check xem là admin hay nhân viên thu ngân.nếu là admin thì hiện thị toàn bộ các tab,còn nếu là nhân viên thu ngân thì chỉ hiển thị đặt sân và thanh toán
2.Xây dựng biểu đồ thống kê
-trong phần core te thêm file StaticManager để gom nhóm thông kê doanh thu cho biểu đồ cột và thời gian đặt cho biểu đồ tròn
-trong phần component ta thêm file giao diện tab static manager vào

