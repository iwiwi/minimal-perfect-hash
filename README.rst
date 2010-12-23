概要
----
最小完全ハッシュ関数の実装です．

アルゴリズムは参考文献の論文のものです．
実際には，ほぼ `Bep <http://www-tsujii.is.s.u-tokyo.ac.jp/~hillbig/bep-j.htm>`_ の bhash の再実装になっていますが，
以下の点が異なります．

* テンプレートで書かれており，std::string 以外の型で使用可能
* キーを保存しない，チェックしない

最小完全ハッシュ関数とは
------------------------
ハッシュ関数が完全であるとは，単射であることです．
ハッシュ関数が最小であるとは，その値域がキーの個数を n として [0, n) の整数であることです．

MinimalPerfectHash クラスは，キーの列を受け取り，最小完全ハッシュ関数を構築します．
その表現に必要な容量は，約 4n ビットです．

使い方
------
sample.cc を見ると分かると思います．

boost の serialization によるファイル等への読み書きに対応していますが，
それ以外の機能は boost が入っていなくても使うことができます．

参考文献
--------
* Daisuke Okanohara. **Bep: Associative Arrays for Very Large Collections.** http://www-tsujii.is.s.u-tokyo.ac.jp/~hillbig/bep-j.htm
* F.C Botelho, R. Pagh, N. Ziviani. **Simple and Space-Efficient Minimal Perfect Hash Functions.** WADS 2007
