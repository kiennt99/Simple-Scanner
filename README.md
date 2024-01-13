# Khái quát về project SimpleScanner



## Chức năng
- Cho phép người dùng lựa chọn một thư mục cần quét qua hộp thoại
- Cho phép người dùng nhập dấu hiệu nhận dạng (signature) bằng hex encoding
- Chương trình sẽ quét tất cả các file trong thư mục người dùng chọn, bao gồm cả các file trong thư mục con
- Lưu danh sách các file mà nội dung có chứa signature người dùng đã nhập, sau đó thông báo cho người dùng
- Hiển thị, cập nhật quá trình quét lên giao diện (giao diện không bị unresponsive)

## Flow cơ bản của chương trình

- Khi chạy chương trình, 1 cửa số hiện ra cho phép người dùng thực hiện các chức năng
- Khi người dùng nhấn "Chọn", hàm ```OnBnClickedSelect``` sử dụng class ```CFolderPickerDialog``` giúp người dùng chọn 1 thư mục bằng Window Explorer. Khi đó, đường dẫn đó sẽ được gán vào biến ```m_directoryPath``` và hiện lên thanh "Tham số đường dẫn"
- Tiếp theo, người dùng có thể nhập 1 mã hex code vào thanh nội dung và nó sẽ được gán vào biến ```m_signature```. Sau đó, nếu nhấn "Bắt đầu" chương trình sẽ sinh ra 1 thread mới để quét file nhờ vào hàm ```ScanDirectory```
  - Hàm ```ScanDirectory``` nhận tham số đầu vào là 2 biến ```m_directoryPath``` và ```m_signature```, hàm này duyệt đệ quy tất cả các file trong thư mục (bao gồm cả thư mục con) và kiểm tra xem mỗi file có chứa signature hay không
- Người dùng cũng có thể chọn ```Quets từ Database```. Khi đó, hàm ```ScanDirectoryWithDatabase``` sẽ lưu tất cả các signature trong database vào 1 vector, sau đó duyệt các file trong thư mục lần lượt với từng signature tương tự như hàm ```ScanDirectory``` (công việc này cũng được thực hiện trong 1 thread mới)
- Sau khi quét xong, thanh "Trạng thái" sẽ hiện ra list tất cả các file trong thư mục, cùng với thời gian khi bắt đầu quét và khi quét xong. Sau đó, 1 cứa số hiện ra để thông báo đã quét xong chương trình, và list các file chứa signature

## Vai trò của đa luồng trong project


   1. Tăng tốc độ xử lý: Bằng cách sử dụng đa luồng, các tác vụ có thể được thực hiện song song trên các luồng khác nhau, giúp tận dụng tài nguyên máy tính và tăng tốc độ xử lý chung của hệ thống.

  2.  Đáp ứng nhanh: Đa luồng cho phép chương trình đáp ứng nhanh hơn đối với các yêu cầu người dùng. Nếu một luồng đang xử lý một tác vụ mà gặp phải trở ngại, các luồng khác vẫn có thể tiếp tục hoạt động mà không bị chặn, cho phép ứng dụng vẫn phản hồi và tiếp tục thực thi các tác vụ khác.


  3.  Xử lý các tác vụ đồng thời: Đa luồng cho phép xử lý đồng thời nhiều tác vụ khác nhau.

  4.  Tối ưu hóa sử dụng tài nguyên: Bằng cách sử dụng đa luồng, có thể tận dụng tối đa tài nguyên máy tính như bộ vi xử lý đa nhân và bộ nhớ. Các tác vụ có thể được phân phối và thực thi trên các luồng khác nhau, giúp tận dụng tài nguyên hiệu quả hơn.



## Điểm cộng và trừ của lập trình đa luồng

### Điểm cộng của lập trình đa luồng

   1. Tăng hiệu năng: Sử dụng đa luồng cho phép chương trình thực hiện nhiều công việc đồng thời, giúp tận dụng tài nguyên hệ thống và tăng tốc độ xử lý. Các tác vụ có thể được chia nhỏ và thực hiện song song, giúp giảm thời gian thực hiện tổng thể của chương trình.
   2.  Đáp ứng tương tác người dùng: Lập trình đa luồng cho phép các tác vụ nền được thực hiện song song với tác vụ giao diện người dùng, giúp người dùng không bị chặn đợi khi chương trình đang xử lý các tác vụ phức tạp.
   3.  Xử lý các tác vụ đồng thời: Lập trình đa luồng cho phép xử lý đồng thời các tác vụ độc lập nhau. Điều này có lợi cho các ứng dụng như trò chơi, ứng dụng đa phương tiện, hoặc các hệ thống thời gian thực.

### Điểm trừ của lập trình đa luồng

  1.  Đồng bộ hóa và xử lý song song: Lập trình đa luồng đòi hỏi kiến thức và kỹ năng cao để đảm bảo đồng bộ hóa đúng và tránh lỗi xung đột dữ liệu. Việc quản lý và đồng bộ các luồng cùng truy cập vào tài nguyên chia sẻ có thể phức tạp và dễ gây ra lỗi nếu không được thực hiện đúng.
  2.  Lỗi race condition: Khi sử dụng đa luồng, một lỗi phổ biến là race condition, trong đó các luồng cạnh tranh với nhau để truy cập và thay đổi dữ liệu chia sẻ. Race condition có thể dẫn đến các kết quả không đoán trước được và gây ra lỗi không thể tái hiện.
  3.  Khó debug: Việc debug trong môi trường đa luồng có thể phức tạp hơn so với lập trình tuần tự. Lỗi xảy ra trong một luồng có thể khó phát hiện và tái hiện, đặc biệt khi liên quan đến các sự kiện xảy ra đồng thời giữa các luồng.

## Cơ chế chống xung đột
Chưa có