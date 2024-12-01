#pragma once

class ArenaAllocator {
public:

      inline explicit ArenaAllocator(size_t bytes) : sz(bytes) {
            buffer = static_cast<std::byte*>(malloc(sz)) ;
            m_offset = buffer ;
      }

      template<typename T>
      inline T* alloc() {
            void* offset = m_offset ;
            m_offset += sizeof(T) ;
            return static_cast<T*>(offset) ;
      }

      /*
      -What these 2 lines do ?
            These two lines disable copying and assignment for the ArenaAllocator object

      -why ?
            If you tried to copy one ArenaAllocator to another or assign one to another,
            it would result in shallow copying (just copying the raw pointer buffer).
            Both objects would point to the same allocated memory,
            which could lead to serious problems like double free errors when both objects try to release the same memory.
      */
      inline ArenaAllocator (const ArenaAllocator& other) = delete ;
      inline ArenaAllocator operator= (const ArenaAllocator& other) = delete ;

      inline ~ArenaAllocator() {
            free(buffer) ;
      }

private:
      size_t sz ;
      std::byte* buffer;
      std::byte* m_offset;
};