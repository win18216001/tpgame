
#pragma  once
  /**
	*	把n的设置为最低位为1的数
	*	n^(n-1)	表示 2^x = n, 求x
	*	n &( n^(n-1) ) 表示把第x位设置为1，其余位为0

	*   同理，求n二进制末尾有多少过0
	*	n & ( n^(n-1)) -1 ;

	*   判断是否为偶数 ( ~n & 1 )
	*/
