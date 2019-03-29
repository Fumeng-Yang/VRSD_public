#ifndef DELETE_PTR_H
#define DELETE_PTR_H
struct delete_ptr   // Helper function to ease cleanup of container
{
   template<typename P>
   void operator ()(P p) {
      delete p;
   }
};
#endif // DELETE_PTR_H
