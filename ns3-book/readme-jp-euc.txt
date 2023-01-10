本書に関するすべてのソースコードはパッケージns3-book.tar.gz に収めている。これらのコード
を利用するには、まず、以下のようにパッケージを適当なところ(たとえば、~/) に展開する。

  % cd ~/
  % tar -zxvf ns3-book.tar.gz

これで ~/ns3-book の下に以下の構成でファイルが展開される。

  ns3-book
  + work
  |  |
  |  +-- local 有線系ネットワーク実験関連コード
  |  |   +-- local/utils 第3 章有線系トポロジー実験用スクリプト
  |  |   +-- local/exp01 第5 章実験1 用スクリプトと関連ツール類
  |  |   +-- local/exp02 第5 章実験2 用スクリプトと関連ツール類
  |  |   +-- ...
  |  |   +-- local/exp10 第5 章実10 用スクリプトと関連ツール類
  |  |   +-- local/rr-queue 第8 章8.3 開発事例2 の検証用スクリプト 
  |  +-- wlan 無線系ネットワーク実験関連コード
  |  |   +-- wlan/utils 第6 章無線系トポロジー実験用スクリプト
  |  |   +-- wlan/exp01 第7 章実験1 用スクリプトと関連ツール類
  |  |   +-- wlan/exp02 第7 章実験2 用スクリプトと関連ツール類
  |  |   +-- ...
  |  | 	+-- wlan/exp07 第7 章実験7 用スクリプトと関連ツール類
  |  | 	+-- wlan/mesh-with-energy 第8 章8.2 開発事例1 の検証用スクリプト
  |  +-- ns3-packegs ns3 関連のパッケージ
  |  +-- wscript-diff wscript に追加した部分
  + install-ns-3.22.sh 
  + .tcshrc
  + readme-jp-euc.txt

これらのコードを以下のように、ns3 のホームディレクトリにコピーして利用する。
  % cd ~/ns3-book
  % cp -r work/local  ns-3.xx/.
  % cp -r work/wlan   ns-3.xx/.

ただし、wlan/exp06 とwlan/mesh-with-energy については、src/mesh/model とsrc/mesh/helper
の修正に関連しているので、修正した後でないと、waf でns3 を構築する際にエラーが発生する。

具体的に修正方法については、第7 章の「実験6」と第8 章の「開発事例1」を参照されたい(wlan/exp06の下
のreadme-jp-euc.txtにも簡単な手順を述べている)。

おすすめ：始めてインストールを行う場合、必要な実験だけをコピーしながらやった方がよいかと思われる。

■ ns-3.24 からパッケージを一括インストールするスクリプトを用意しました(wlan/exp06の修正も対応済)
	(1) ns3-3.24.1 をインストール：   sh install-ns3.sh
	(2) 付属パッケージをインストール：sh install-my-ns3-packages.sh

検証済みバージョン：
	ns-3.15, ns-3.16, ns-3.17, ns-3.18, ns-3.18.1 , ns-3.18.2, ns-3.19, ns-3.20, ns-3.21
	ns-3.22, ns-3.23, ns-3.24, ns-3.24.1
	なお、ns-3.17, ns-3.23からwscriptの記述方法は少し変わった。

	Ubuntu 12.04 LTS, Ubuntu 13.04, Ubuntu 13.10 JP, Ubuntu 14.04 LTS(32/64)




