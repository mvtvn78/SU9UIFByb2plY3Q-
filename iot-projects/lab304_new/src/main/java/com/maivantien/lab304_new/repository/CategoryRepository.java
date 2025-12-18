package com.maivantien.lab304_new.repository;
import org.springframework.data.jpa.repository.JpaRepository;
import com.maivantien.lab304_new.entity.Category;
public interface CategoryRepository extends JpaRepository<Category, Long> {
}