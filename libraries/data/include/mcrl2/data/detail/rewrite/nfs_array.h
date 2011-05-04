namespace mcrl2
{
namespace data
{
namespace detail
{

class nfs_array
{
public:
  nfs_array(size_t size) : m_array(new size_t[size])
  {
  }
  ~nfs_array()
  {
    delete[] m_array;
  }
  void clear(size_t arity)
  {
    if (arity > 0)
    {
      memset(m_array, 0, ((arity-1)/(sizeof(size_t)*8)+1)*sizeof(size_t));
    }
  }
  void fill(size_t arity, bool val = true)
  {
    size_t newval = val ? (~((size_t)0)) : 0;
    for(size_t i = 0; i * sizeof(size_t) * 8 < arity; ++i)
    {
      m_array[i] = newval;
    }
  }
  size_t get_value(size_t arity)
  {
    assert(arity <= NF_MAX_ARITY);
    return m_array[0] & (((size_t)1 << arity) - 1);
  }
  void set_value(size_t arity, size_t val)
  {
    assert(arity <= NF_MAX_ARITY || val == 0);
    m_array[0] = val;
  }
  bool equals(nfs_array& other, size_t arity)
  {
    size_t i = 0;
    while (arity >= sizeof(size_t)*8)
    {
      if (m_array[i] != other.m_array[i])
      {
        return false;
      }
      arity -= sizeof(size_t)*8;
      ++i;
    }
    return (m_array[i] & ((1 << arity)-1)) == (other.m_array[i] & ((1 << arity)-1));
  }
  bool get(size_t i)
  {
    return m_array[i/(sizeof(size_t)*8)] & (((size_t) 1) << (i%(sizeof(size_t)*8)));
  }
  void set(size_t i, bool val = true)
  {
    if (val)
    {
      m_array[i/(sizeof(size_t)*8)] |= ((size_t) 1) << (i%(sizeof(size_t)*8));
    }
    else
    {
      m_array[i/(sizeof(size_t)*8)] &= ~(((size_t) 1) << (i%(sizeof(size_t)*8)));
    }
  }
  bool is_clear(size_t arity)
  {
    size_t i = 0;
    while (arity >= sizeof(size_t)*8)
    {
      if (m_array[i++] != ((size_t) 0))
      {
        return false;
      }
      arity -= sizeof(size_t)*8;
    }
    return (m_array[i] & ((1 << arity)-1)) == 0;
  }
  bool is_filled(size_t arity)
  {
    size_t i = 0;
    while (arity >= sizeof(size_t)*8)
    {
      if (m_array[i++] != ~((size_t) 0))
      {
        return false;
      }
      arity -= sizeof(size_t)*8;
    }
    return (m_array[i] & ((1 << arity)-1)) == (size_t)((1 << arity)-1);
  }
  size_t getraw(size_t i)
  {
    return m_array[i];
  }
private:
  size_t *m_array;
};

}
}
}
