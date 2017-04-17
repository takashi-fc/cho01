# Charlesを使ってプロキシサーバをローカルに構築する

## iOS開発での通信デバッグ
通信処理を行うアプリの開発をしていると、当然ながら通信処理に関するデバッグが必要になってくる。
Android開発ではstetoを使えば簡単に通信ログが確認できるが、iOS開発ではこれがなかなか面倒くさい。
最近ではCharlesというツールが便利なようなので触ってみた。

## Charlesのセットアップ
Charlesは下記の公式URLからダウンロードできる。
@<href>{https://www.charlesproxy.com/download/}

Charlesの初回起動時、Charlesを起動している間はプロキシの設定をCharlesのものに切り替えるかどうか聞かれるので、「Grant Privileges」を選択して設定を行う。
![起動時設定](images/chales01.png)



そのあと、Firefoxのアドオンもインストールするか聞かれるが、これは利用したい場合のみ「Install」を選択すれば良い。
![Firefoxアドオンの選択](images/chales02.png)

Charlesをプロキシサーバとして利用するため、Mac本体とiPhoneは同じWifiに接続しておくこと。

## Macの設定
Charlesのメニューから「Proxy -> SSL Proxying Settings」を選択し、開かれた画面でAddボタンを押下して「Host」に * を入力してOKボタンで完了。
![ワイルドカード設定](images/chales04.png)

同じくCharlesのメニューから「Help -> SSL Proxying -> Install Charles Root Certificate」を選択して証明書をMacにインストールする。
![証明書インストール](images/chales03.png)

キーチェーンツールを起動すると「Charles Proxy Custom Root Certificate」がインストールされていることがわかる。
この時、「このルート証明書は信頼されていません」と表示される場合があるが、「Charles Proxy Custom Root Certificate」をダブルクリック -> 信頼タブを開く -> 常に信頼するを選択、で証明書を信頼できるように設定することができる。

## iPhoneの設定
MacのIPアドレスはCharlesのメニュー「Help -> Local IP Address」で確認できる。ポートはデフォルト8888で設定されているので、このIPアドレスとポートをiPhone本体ののWifi設定からHTTPプロキシ設定に入力する。
![Wifi設定](images/chales05.png)

この状態でiPhoneで通信を発生させるとCharles側で確認ダイアログが表示されるので「Allow」を選択する。

最後に下記のURLにiPhoneでアクセスし、本体にプロファイルをインストールする。
@<href>{http://www.charlesproxy.com/getssl}
