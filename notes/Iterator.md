# iterator与iterator_traits

## iterator

迭代器中需要定义以下类型：

- `value_type`: 迭代器指向的类型
- `difference_type`: 两个迭代器之间的距离
- `pointer`: 指向`value_type`的指针
- `reference`: 对`value_type`的引用
- `iterator_category`: 迭代器的类别，例如`input_iterator_tag`

迭代器分为以下几种：

- `input_iterator`输入迭代器，满足：
  - 可复制(CopyConstructible, CopyAssignable)，可销毁(Destructible)
  - 支持解引用：`operator*`返回可读的值或引用
  - 支持前置自增`++it`与后置自增`it++`
  -  支持相等于不等比较(`operator==`与`operator!=`)

- `output_iterator`输出迭代器，满足：
  - 支持前置自增`++it`与后置自增`it++`
  - 允许通过`*it = value`写入

- `forward_iterator`前向迭代器，满足：
  - 满足输入和输出迭代器的所有要求
  - 可以遍历同一个序列，可以有多个迭代器指向不同位置
  - 必须是默认构造的

- `bidirectional_iterator`双向迭代器，满足：
  - 满足前向迭代器全部要求
  - 支持前置递减`--it`和后置递减`it--`
- `random_access_iterator`随机访问迭代器，满足：
  - 满足双向迭代器的所有要求
  - 常数时间的元素访问和跳转
  - 支持算术运算：`+`, `-`, `+=`, `-=`
  - 支持比较运算：`<`, `>`, `<=`, `>=`
  - 支持下标操作直接访问特定位置元素`it[n]`