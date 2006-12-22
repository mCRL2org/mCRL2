#ifndef CONEDB_H
#define CONEDB_H

class ConeDB {
  public:
    ConeDB(int hashclass,int blocksize);
    ~ConeDB();
    void addCone(int k,unsigned char tb,int c);
    int  findCone(int k,unsigned char tb);
  private:
    struct cone_bucket {
      int key;
      int cone;
      int next;
      unsigned char top_bot;
    };
    int *hashtable;
    int hashmask;
    cone_bucket *buckets;
    int bucket_size;
    int bucket_block;
    int bucket_next;

    void check_buckets();
    int  find_bucket(int k,unsigned char tb);
};

#endif
