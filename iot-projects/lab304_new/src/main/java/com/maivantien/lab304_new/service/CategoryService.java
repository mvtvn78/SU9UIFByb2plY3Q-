package com.maivantien.lab304_new.service;
import java.util.List;
import com.maivantien.lab304_new.entity.Category;
public interface CategoryService {
    Category createCategory(Category category);
    Category getCategoryById(Long categoryId);
    List<Category> getAllCategories();
    Category updateCategory(Category category);
    void deleteCategory(Long categoryId);
}
